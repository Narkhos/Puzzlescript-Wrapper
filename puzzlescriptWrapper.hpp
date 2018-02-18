/* ***********************************************************************************
 * Copyright (C) 2018 Xavier Direz - http://www.LaFaceObscureDuPixel.fr              *
 *                                                                                   *
 * This file is part of PuzzlescriptWrapper.                                         *
 *                                                                                   *
 * PuzzlescriptWrapper is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU Lesser General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * PuzzlescriptWrapper is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU Lesser General Public License for more details.                               *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public License          *
 * along with  this program; If not, see <http://www.gnu.org/licenses/>.             *
 *************************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <vector>
#include <map>
#include "duktape/duktape.h"
#include <SDL2/SDL.h>
#include "display.hpp"

#include "sfxr.hpp"

using namespace std;

struct spriteInstance
{
    int id;
    int x;
    int y;
};

struct rgbColor
{
    int r;
    int g;
    int b;
};

Engine* e;
class PuzzlescriptSprite
{
    public:
    SDL_Surface* surface;
    int textureId;
    bool toUpdate;

    PuzzlescriptSprite(int widht, int height)
        :textureId(-1), toUpdate(true)
    {
        this->surface = SDL_CreateRGBSurface(0, widht, height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, 0, 0, 0));
    }
    ~PuzzlescriptSprite()
    {
        deleteTexture();
        SDL_FreeSurface(this->surface);
        this->surface = NULL;
    }

    void deleteTexture()
    {
        glDeleteTextures(1,(GLuint*)&(this->textureId));
        this->textureId = -1;
    }

    void updateTexture()
    {
        if(this->toUpdate)
        {
            this->toUpdate = false;
            if(this->textureId != -1)
            {
                this->deleteTexture();
            }
            this->textureId = SurfaceToTexture2(this->surface);
        }
    }


    void draw(int x, int y, int w, int h)
    {
       updateTexture();
       if(this->textureId != -1)
       {
           drawImage(this->textureId, x, y, w, h, 1.0);
       }
    }

};

vector<PuzzlescriptSprite*> sprites;
vector<spriteInstance> instances;

int cellWidth;
int cellHeight;
int puzzlescriptInterval;
rgbColor backgroundColor;
string gameTitle;

bool sfxOn;
bool showFPS = false;

map<unsigned int, SfxrSound*> soundList;

rgbColor webColorToRGB(const char* fillStyle)
{
    rgbColor rgb;
    string tmp(fillStyle);
    string r = tmp.substr(1,2);
    string g = tmp.substr(3,2);
    string b = tmp.substr(5,2);

    stringstream r_ss;
    r_ss << std::hex << r;
    r_ss >> rgb.r;

    stringstream g_ss;
    g_ss << std::hex << g;
    g_ss >> rgb.g;

    stringstream b_ss;
    b_ss << std::hex << b;
    b_ss >> rgb.b;

    //cout << fillStyle << " : " << r << g << b << " : " << rgb.r << ", "<< rgb.g << ", "<< rgb.b <<endl;
    return rgb;
}

string getLocalStorage(duk_context *ctx)
{
    string localStorageData;
    duk_eval_string(ctx, "localStorage.serialize()");
    localStorageData = duk_get_string(ctx, -1);
    duk_pop(ctx);
    return localStorageData;
}

void saveGameState(string localStorageFile, duk_context *ctx)
{
    string localStorageData = getLocalStorage(ctx);
    // Save to file
    ofstream storage(localStorageFile);
    if(storage)
    {
        storage << localStorageData << endl;
        storage.close();
    }
}

void loadGameState(string localStorageFile, duk_context *ctx)
{
    // Load data from file to javascript context
    ifstream storage(localStorageFile);
    if (storage)
    {
        string localStorageData = "";

        storage.seekg(0, ios::end);
        localStorageData.reserve(storage.tellg());
        storage.seekg(0, ios::beg);

        localStorageData.assign((istreambuf_iterator<char>(storage)),
                    istreambuf_iterator<char>());

        duk_eval_string_noresult(ctx, localStorageData.c_str());
        storage.close();
    }
}

string getTitle(duk_context *ctx)
{
    duk_eval_string(ctx,"state.metadata.title");
    gameTitle = duk_get_string(ctx, -1);
    duk_pop(ctx);
    return gameTitle;
}

rgbColor getBackgroundColor(duk_context *ctx)
{
    duk_eval_string(ctx,"document.body.style.backgroundColor");

    string bg = duk_get_string(ctx, -1);
    backgroundColor = webColorToRGB(bg.c_str());

    duk_pop(ctx);
    return backgroundColor;
}

void updateCellWidth(duk_context *ctx)
{
    duk_eval_string(ctx,"cellwidth");
    cellWidth = duk_get_number(ctx, -1);
    duk_pop(ctx);
}

void updateCellHeight(duk_context *ctx)
{
    duk_eval_string(ctx,"cellheight");
    cellHeight = duk_get_number(ctx, -1);
    duk_pop(ctx);
}

void updateDeltaTime(duk_context *ctx)
{
    duk_eval_string(ctx,"deltatime");
    puzzlescriptInterval = duk_get_number(ctx, -1);
    duk_pop(ctx);
}

bool getQuittingTitleScreen(duk_context *ctx)
{
    duk_eval_string(ctx,"quittingTitleScreen");
    bool quittingTitleScreen = duk_get_boolean(ctx, -1);
    duk_pop(ctx);
    return quittingTitleScreen;
}

bool getIsTitleScreen(duk_context *ctx)
{
    duk_eval_string(ctx,"titleScreen");
    bool isTitleScreen = duk_get_boolean(ctx, -1);
    duk_pop(ctx);
    return isTitleScreen;
}

/* For brevity assumes a maximum file length of 16kB. */
static void push_file_as_string(duk_context *ctx, const char *filename) {
    FILE *f;
    size_t len;
    char buf[1638400]; // TODO : remplacer par un chargement des données en c++ avec une taille de buffer adaptative

    f = fopen(filename, "rb");
    if (f) {
        len = fread((void *) buf, 1, sizeof(buf), f);
        fclose(f);
        duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
    } else {
        duk_push_undefined(ctx);
    }
}

static duk_ret_t native_print(duk_context *ctx) {
    duk_push_string(ctx, " ");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);
    printf("%s\n", duk_to_string(ctx, -1));
    return 0;
}

static duk_ret_t nativePlaySound(duk_context *ctx)
{
    if(sfxOn)
    {
        unsigned int seed = duk_require_int(ctx,0);

        soundList[seed]->playSample();
    }
    return 0;
}

static duk_ret_t nativeGenerateSound(duk_context *ctx)
{
    if(sfxOn)
    {
        unsigned int seed = duk_require_int(ctx,0);
        soundList[seed] = new SfxrSound(
            // Envelope
            (float)duk_require_number(ctx,1),
            (float)duk_require_number(ctx,2),
            (float)duk_require_number(ctx,3),
            (float)duk_require_number(ctx,4),

            // Tone
            (float)duk_require_number(ctx,5),
            (float)duk_require_number(ctx,6),
            (float)duk_require_number(ctx,7),
            (float)duk_require_number(ctx,8),

            // Vibrato
            (float)duk_require_number(ctx,9),
            (float)duk_require_number(ctx,10),

            // Tonal change
            (float)duk_require_number(ctx,11),
            (float)duk_require_number(ctx,12),

            // Duty
            (float)duk_require_number(ctx,13),
            (float)duk_require_number(ctx,14),

            // Repeat
            (float)duk_require_number(ctx,15),

            // Phaser
            (float)duk_require_number(ctx,16),
            (float)duk_require_number(ctx,17),

            // Low-pass filter
            (float)duk_require_number(ctx,18),
            (float)duk_require_number(ctx,19),
            (float)duk_require_number(ctx,20),

            // High-pass filter
            (float)duk_require_number(ctx,21),
            (float)duk_require_number(ctx,22),

            // Main parameters
            (float)duk_require_number(ctx,23),
            duk_require_int(ctx,24),
            duk_require_int(ctx,25),
            duk_require_int(ctx,26)
        );
        soundList[seed]->generateSound();
    }
    return 0;
}


static duk_ret_t nativeFillRect(duk_context *ctx) {
	int contextId = duk_require_int(ctx, 0);
	const char *fillStyle = duk_require_string(ctx, 1);
    int x = duk_require_int(ctx, 2);
    int y = duk_require_int(ctx, 3);
    int w = duk_require_int(ctx, 4);
    int h = duk_require_int(ctx, 5);

    if(string(fillStyle) != string("transparent"))
    {
        SDL_Rect r;
        r.x=x;r.y=y;r.w=w;r.h=h;
        SDL_Surface * s = sprites[contextId]->surface;

        // convert web style color string to RGB
        rgbColor color = webColorToRGB(fillStyle);
        SDL_FillRect(s, &r, SDL_MapRGBA(s->format, color.r, color.g, color.b, 255));
    }
    return 0;
}

static duk_ret_t nativeClearRect(duk_context *ctx) {
	int contextId = duk_require_int(ctx, 0);
    int x = duk_require_int(ctx, 1);
    int y = duk_require_int(ctx, 2);
    int w = duk_require_int(ctx, 3);
    int h = duk_require_int(ctx, 4);

    SDL_Rect r;
    r.x=x;r.y=y;r.w=w;r.h=h;
    SDL_Surface * s = sprites[contextId]->surface;

    SDL_FillRect(s, &r, SDL_MapRGBA(s->format, 0, 0, 0, 0));

    return 0;
}

static duk_ret_t drawImageNative(duk_context *ctx) {
	int imageId = duk_require_int(ctx, 0);
    int x = duk_require_int(ctx, 1);
    int y = duk_require_int(ctx, 2);

    spriteInstance si;

    si.id=imageId;
    si.x=x;
    si.y=y;

    instances.push_back(si);

    return 0;
}

static duk_ret_t getWidth(duk_context *ctx)
{
    duk_push_int(ctx,e->display->current_width);
    return 1;
}

static duk_ret_t getHeight(duk_context *ctx)
{
    duk_push_int(ctx,e->display->current_height);
    return 1;
}

static duk_ret_t addNativeContext(duk_context *ctx)
{
    int w = duk_require_int(ctx, 0);
    int h = duk_require_int(ctx, 1);
	//static int nativeId=0;
    //duk_push_int(ctx,++nativeId);


	sprites.push_back(new PuzzlescriptSprite(w,h));

    duk_push_int(ctx,sprites.size()-1);
    //cout << "sprite : " << sprites.size() << endl;
    return 1;
}


static duk_ret_t nativeClearInstances(duk_context *ctx)
{
    instances.erase(instances.begin(), instances.end());
    return 1;
}

bool loadFile(duk_context *ctx, const char *filename)
{
	push_file_as_string(ctx, filename);
    if (duk_peval(ctx) != 0) {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
        return false;
    }
    duk_pop(ctx);  /* ignore result */
	return true;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string loadGame(string gameFile)
{
    ifstream t(gameFile);
    string game;

    t.seekg(0, ios::end);
    game.reserve(t.tellg());
    t.seekg(0, ios::beg);

    game.assign((istreambuf_iterator<char>(t)),
                istreambuf_iterator<char>());
    stringstream ss;

    replaceAll(game,"\r","");
    replaceAll(game,"\n","\\n");
    replaceAll(game,"\"","\\\"");

    ss << "var sourceCode = \"" << game << "\";\ncompile([\"restart\"],sourceCode);";

    return ss.str();
}

void updatePuzzlescript(duk_context * ctx, int dt)
{
    static int cumulativeTime = 0;
    cumulativeTime += dt;
    if(cumulativeTime >= puzzlescriptInterval)
    {
        duk_eval_string_noresult(ctx, "update()");
        updateCellWidth(ctx);
        updateCellHeight(ctx);
        cumulativeTime = 0;
    }
}

void calculateFPS(Engine* e, GUI_TexteDynamique& txtFPS, SDL_Color& textColor)
{
    static int frame = 0;
    frame++;

    static int prec_time = 0;

    int deltatime = e->currentTime-prec_time;

    if(deltatime >= 1000)
    {
        wstringstream ss;
        ss << L"FPS : " << frame;
        txtFPS.setTexte(ss.str());
        prec_time = e->currentTime;
        frame = 0;
    }

}

void run(duk_context *ctx)
{
    SDL_Event event;
    int deltaTime = 0;
    SDL_Color textColor = {255 , 255 , 255, 255};
    GUI_TexteDynamique txtFPS(L"FPS : 0", e->fontPool.getFont("OxygenMono-Regular.ttf"), textColor);

    initSfxr();

    SDL_SetWindowTitle(e->display->screen, gameTitle.c_str());

    getBackgroundColor(ctx);
    glClearColor((float)backgroundColor.r/255.0,(float)backgroundColor.g/255.0,(float)backgroundColor.b/255.0,1.0);

    updateDeltaTime(ctx);

    //bool firstLoop = true;

    // BOUCLE PRINCIPALE
	while (e->running)
	{
	    // ACTUALISATION DU CONTEXT
	    e->update();
	    deltaTime = e->getDeltaTime();

	    if(showFPS)
	    {
            calculateFPS(e, txtFPS, textColor);
        }

        updatePuzzlescript(ctx, deltaTime);

        while( SDL_PollEvent(&event))
        {
            if( event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                cout << "resize : " << event.window.data1 << ", "<< event.window.data2<< endl;
                glViewport( 0, 0, event.window.data1, event.window.data2 );
                glLoadIdentity();

                glOrtho(0, e->display->width, e->display->height, 0, -1, 1);
                e->display->current_width = event.window.data1;
                e->display->current_height = event.window.data2;

                updateCellWidth(ctx);
                updateCellHeight(ctx);
            }

            switch(event.type)
            {
                case SDL_QUIT:
                    e->running = false;
                break;
                case SDL_KEYDOWN :
                    if( event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN
                         || event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_x)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 13})");
                    }

                    if( event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 37})");
                    }

                    if( event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 39})");
                    }
                    if( event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 38})");
                    }

                    if( event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 40})");
                    }
                    if( event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_z)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 85})");
                    }
                    if( event.key.keysym.sym == SDLK_r)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 82})");
                    }
                    if( event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        if(getIsTitleScreen(ctx))
                        {
                            e->running = false;
                        }
                        else
                        {
                            duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 27})");
                        }

                    }
                    if( event.key.keysym.sym == SDLK_e)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 69})");
                    }
                    if( event.key.keysym.sym == SDLK_0)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 48})");
                    }
                    if( event.key.keysym.sym == SDLK_1)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 49})");
                    }
                    if( event.key.keysym.sym == SDLK_2)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 50})");
                    }
                    if( event.key.keysym.sym == SDLK_3)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 51})");
                    }
                    if( event.key.keysym.sym == SDLK_4)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 52})");
                    }
                    if( event.key.keysym.sym == SDLK_5)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 53})");
                    }
                    if( event.key.keysym.sym == SDLK_6)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 54})");
                    }
                    if( event.key.keysym.sym == SDLK_7)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 55})");
                    }
                    if( event.key.keysym.sym == SDLK_8)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 56})");
                    }
                    if( event.key.keysym.sym == SDLK_9)
                    {
                        duk_eval_string_noresult(ctx, "onKeyDown({keyCode: 57})");
                    }

                    if(event.key.keysym.sym == SDLK_TAB)
                    {
                        showFPS = !showFPS;
                    }

                break;
                case SDL_KEYUP:
                    if( event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN
                         || event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_x)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 13})");
                    }

                    if( event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 37})");
                    }

                    if( event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 39})");
                    }
                    if( event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 38})");
                    }

                    if( event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 40})");
                    }
                    if( event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_z)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 85})");
                    }
                    if( event.key.keysym.sym == SDLK_r)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 82})");
                    }
                    if( event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 27})");
                    }
                    if( event.key.keysym.sym == SDLK_e)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 69})");
                    }
                    if( event.key.keysym.sym == SDLK_0)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 48})");
                    }
                    if( event.key.keysym.sym == SDLK_1)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 49})");
                    }
                    if( event.key.keysym.sym == SDLK_2)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 50})");
                    }
                    if( event.key.keysym.sym == SDLK_3)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 51})");
                    }
                    if( event.key.keysym.sym == SDLK_4)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 52})");
                    }
                    if( event.key.keysym.sym == SDLK_5)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 53})");
                    }
                    if( event.key.keysym.sym == SDLK_6)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 54})");
                    }
                    if( event.key.keysym.sym == SDLK_7)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 55})");
                    }
                    if( event.key.keysym.sym == SDLK_8)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 56})");
                    }
                    if( event.key.keysym.sym == SDLK_9)
                    {
                        duk_eval_string_noresult(ctx, "onKeyUp({keyCode: 57})");
                    }
                break;
            }
        }

        // GESTION DE L'AFFICHAGE
        glClear(GL_COLOR_BUFFER_BIT);

        /*
        // Display all sprites
        for(int i = 0; i < sprites.size(); i++)
        {
            sprites[i]->draw(i%34 * 30, i/34 * 30, 30, 30);
        }
        */

        for(int i = 0; i<instances.size();i++)
        {
            sprites[instances[i].id]->draw(instances[i].x, instances[i].y, cellWidth, cellHeight);
        }

        if(showFPS)
	    {
            txtFPS.draw(0,0);
        }

		glFlush();
		SDL_GL_SwapWindow(e->display->screen);

        /*if(firstLoop)
        {
            cout << "firstloop" << endl;
            glViewport( 0, 0, e->display->width, e->display->height );
            glLoadIdentity();
            glOrtho(0, e->display->width, e->display->height, 0, -1, 1);
            firstLoop = false;
        }*/
    }
}

// Fatal Error manager for Duktape javascript engine
static void my_fatal(void *udata, const char *msg) {
    (void) udata;  /* ignored in this case, silence warning */

    /* Note that 'msg' may be NULL. */
    cerr << "*** FATAL ERROR: " << (msg ? msg : "no message") << endl;;
    abort();
}

void puzzlescriptWrapper(Engine& engine, string gameFile)
{
    e = &engine;
    duk_context *ctx = NULL;
    sfxOn = e->cfg.sfx;

    // Choose normal or minified puzzlescript source
    #ifdef __USE_MINIFIED__
        string puzzlescriptPath("data/js/puzzlescript/minified/");
    #else
        string puzzlescriptPath("data/js/puzzlescript/js/");
    #endif

    string jsPath("data/js/");

    ctx = duk_create_heap(NULL, NULL, NULL, NULL, my_fatal);
    if (!ctx) {
        printf("Failed to create a Duktape heap.\n");
        exit(1);
    }

    duk_push_global_object(ctx);
    duk_push_c_function(ctx, native_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "print");

	duk_push_c_function(ctx, getWidth, 0);
    duk_put_prop_string(ctx, -2, "getWidth");

	duk_push_c_function(ctx, getHeight, 0);
    duk_put_prop_string(ctx, -2, "getHeight");

	duk_push_c_function(ctx, addNativeContext, 2);
    duk_put_prop_string(ctx, -2, "addNativeContext");

	duk_push_c_function(ctx, nativeClearInstances, 0);
    duk_put_prop_string(ctx, -2, "nativeClearInstances");

	duk_push_c_function(ctx, nativePlaySound, 1);
    duk_put_prop_string(ctx, -2, "nativePlaySound");

	duk_push_c_function(ctx, nativeGenerateSound, 27);
    duk_put_prop_string(ctx, -2, "nativeGenerateSound");

	duk_push_c_function(ctx, nativeFillRect, 6);
    duk_put_prop_string(ctx, -2, "nativeFillRect");

	duk_push_c_function(ctx, nativeClearRect, 5);
    duk_put_prop_string(ctx, -2, "nativeClearRect");

    duk_push_c_function(ctx, drawImageNative, 3);
    duk_put_prop_string(ctx, -2, "drawImageNative");

	// Loading emulated dependencies
	if(!loadFile(ctx, (jsPath + string("navigator.js")).c_str())) goto finished;
	if(!loadFile(ctx, (jsPath + string("window.js")).c_str())) goto finished;

    loadGameState("./data/localStorage/" + gameFile, ctx);

	if(!loadFile(ctx, (jsPath + string("document.js")).c_str())) goto finished;

	// Loading puzzlescript source code
	if(!loadFile(ctx, (puzzlescriptPath + string("globalVariables.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("debug_off.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("font.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("rng.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("riffwave.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("sfxr.js")).c_str())) goto finished; // sound management

	if(!loadFile(ctx, (jsPath + string("sfxr.js")).c_str())) goto finished;
	if(!loadFile(ctx, (jsPath + string("codemirror.js")).c_str())) goto finished;

	if(!loadFile(ctx, (puzzlescriptPath + string("colors.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("graphics.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("engine.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("parser.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("compiler.js")).c_str())) goto finished;
	if(!loadFile(ctx, (puzzlescriptPath + string("inputoutput.js")).c_str())) goto finished;

    if(!loadFile(ctx, (jsPath + string("redrawUpgrade.js")).c_str())) goto finished;

	// Testing ps modules
	//if(!loadFile(ctx, puzzlescriptPath + string("test.js"))) goto finished;

    duk_eval_string_noresult(ctx,loadGame("./data/games/" + gameFile).c_str());

    getTitle(ctx);

    updateCellWidth(ctx);
    updateCellHeight(ctx);

	run(ctx);

	saveGameState("./data/localStorage/" + gameFile, ctx);

 finished:
    duk_destroy_heap(ctx);
}
