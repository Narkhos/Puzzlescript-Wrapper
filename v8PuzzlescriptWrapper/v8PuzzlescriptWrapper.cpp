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

#ifndef __PS_WRAPPER_VERSION__
#define __PS_WRAPPER_VERSION__ "v2.0.3"
#endif

// #define __USE_MINIFIED__
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <GL/glew.h>
#include <v8.h>
#include <libplatform/libplatform.h>
#include "gameEngine/sfxr.hpp"
#include "gameEngine/gui.hpp"
#include "gameEngine/json.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

using namespace std;

SDL_Window* window;

struct rgbColor
{
	int r;
	int g;
	int b;
};

class DiscreetGauge
{
	public:
		vector<GUI_Button> buttonList;
		TTF_Font* font;
		GLuint labelTexId;
		float x;
		float y;
		int value;
		wstring label;
		int label_w;
		int label_h;
		rgbColor textColor;

		DiscreetGauge(
			int initValue,
			int textureButton,
			int textureButtonZero,
			float _x,
			float _y,
			float buttonWidth,
			float buttonHeight,
			int length,
			TTF_Font* _font,
			wstring _label,
			rgbColor _textColor)
			:value(initValue), x(_x), y(_y), font(_font), textColor(_textColor), label(_label)
		{
			SDL_Color couleur = { textColor.r , textColor.g , textColor.b, 255 };
			labelTexId = texteToTexture(font, couleur, _label, this->label_w, this->label_h);

			int idTextureButton = textureButtonZero;
			for (int i = 0; i < length; i++)
			{
				buttonList.push_back(GUI_Button(idTextureButton, _x + (i * buttonWidth), _y + this->label_h, buttonWidth, buttonHeight, 2));
				idTextureButton = textureButton;
			}

			this->updateButtons();
		}

		void updateTextColor(const rgbColor &newColor)
		{
			if (newColor.r != this->textColor.r
				|| newColor.g != this->textColor.g
				|| newColor.b != this->textColor.b)
			{
				this->textColor = newColor;
				SDL_Color couleur = { textColor.r , textColor.g , textColor.b, 255 };
				glDeleteTextures(1, &labelTexId);
				labelTexId = texteToTexture(font, couleur, this->label, this->label_w, this->label_h);
			}
		}

		void updateButtons()
		{
			// Change state of each button in the gauge
			for (int i = 0; i < this->buttonList.size(); i++)
			{
				buttonList[i].setEtat(i <= this->value ? 0 : 1);
			}
		}

		int onClick(int mouse_x, int mouse_y)
		{
			int newIndex = -1;
			for (int i = 0; i < this->buttonList.size(); i++)
			{
				if (this->buttonList[i].isIn(mouse_x, mouse_y))
				{
					newIndex = i;
					break;
				}
			}

			if (newIndex == -1)
			{
				return -1; // The user does not clicked this gauge
			}

			this->value = newIndex;

			this->updateButtons();

			return this->value; // return gauge value (changed or not)
		}

		void draw()
		{
			drawImage(labelTexId, x, y, (float)this->label_w, (float)this->label_h, 1.0f);
			for (int i = 0; i < this->buttonList.size(); i++)
			{
				buttonList[i].draw();
			}
		}

};

class Config
{
	public:
		string filepath;
		nlohmann::json data;

		Config(string _filepath)
			:filepath(_filepath)
		{
			ifstream i(this->filepath);
			
			i >> data;
		}

		void save()
		{
			ofstream o(filepath);
			o << setw(4) << data << endl;
		}

		void trace()
		{
			cout << data << endl;
		}
};

TTF_Font* font = nullptr;

TTF_Font* loadFont(string filepath)
{
	// INITIALISATION de SDL_ttf
	if (!TTF_WasInit() && TTF_Init() == -1)
	{
		cerr << "TTF_Init : " << TTF_GetError() << endl;
		return NULL;
	}

	TTF_Font* font = TTF_OpenFont(filepath.c_str(), 16);

	if (!font)
	{
		cerr << "TTF_OpenFont : " << TTF_GetError() << endl;
		return NULL;
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	return font;
}

struct spriteInstance
{
	int id;
	int x;
	int y;
};

class PuzzlescriptSprite
{
public:
	SDL_Surface* surface;
	int textureId;
	bool toUpdate;

	PuzzlescriptSprite(int width, int height)
		:textureId(-1), surface(NULL)
	{
		this->init(width, height);
	}

	~PuzzlescriptSprite()
	{
		deleteTexture();
		SDL_FreeSurface(this->surface);
		this->surface = NULL;
	}

	void init(int width, int height)
	{
		this->toUpdate = true;
		this->deleteTexture();
		SDL_FreeSurface(this->surface);
		this->surface = NULL;
		this->surface = SDL_CreateRGBSurface(0, width, height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, 0, 0, 0));
	}

	void deleteTexture()
	{
		if (this->textureId != -1)
		{
			glDeleteTextures(1, (GLuint*)&(this->textureId));
			this->textureId = -1;
		}
	}

	void updateTexture()
	{
		if (this->toUpdate)
		{
			this->toUpdate = false;
			if (this->textureId != -1)
			{
				this->deleteTexture();
			}

			this->textureId = SurfaceToTexture(this->surface);
		}
	}


	void draw(int x, int y, int w, int h)
	{
		updateTexture();

		if (this->textureId != -1)
		{
			drawImage(this->textureId, (float)x, (float)y, (float)w, (float)h, 1.0f);
		}
	}
};

int chunk_volume_index = 8;
int chunk_volume_scale[] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128};

int sfxr_volume_index = 8;
float sfxr_volume_scale[] = {0.0f, 0.05f, 0.10f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.75f, 0.8f};

class Sfx
{
	public:
		Mix_Chunk* chunk;
		SfxrSound* sfxrSound;

	Sfx(const string seed, SfxrSound* _sfxrSound)
	{
		string supportedFormatList[] = {"mp3", "ogg", "wav", "aiff", "voc","mod", "xm", "s3m", "669", "it", "med", "mid" };
		for (int i = 0; i < 12; i++)
		{
			stringstream ss;
			ss << "./data/sfx/" << seed << "." << supportedFormatList[i];
			this->chunk = Mix_LoadWAV(ss.str().c_str());

			if (this->chunk != nullptr) break;
		}

		this->sfxrSound = _sfxrSound;

		/*if (this->chunk == nullptr)
		{
			this->chunk = this->sfxrSound->generateSound();
		}*/

	}

	~Sfx()
	{
		if (this->chunk != nullptr) Mix_FreeChunk(this->chunk);
		if (this->sfxrSound != nullptr) delete this->sfxrSound;
	}

	void playSample()
	{
		if (this->chunk != nullptr)
		{
			Mix_VolumeChunk(chunk, chunk_volume_scale[chunk_volume_index]);
			Mix_PlayChannel(-1, this->chunk, 0);
		}
		else
		{
			this->sfxrSound->playSample();
		}
	}
};

vector<PuzzlescriptSprite*> sprites;
vector<spriteInstance> instances;

int width = 1024;
int height = 768;
int windowedWidth = 1024;
int windowedHeight = 768;
bool fullscreen = false;
int showMouseHud = 0;
int MouseHudDelay = 2000;

int cellWidth;
int cellHeight;
int puzzlescriptInterval;
rgbColor backgroundColor;
rgbColor foregroundColor;
string gameTitle;

bool sfxOn=true;
bool musicOn=true;
bool showFPS = false;

Mix_Music *music = nullptr;
int music_volume_index = 8;
int music_volume_scale[] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128};

int getMusicVolume()
{
	return (musicOn == true) ? music_volume_scale[music_volume_index] : 0;
}

map<unsigned int, Sfx*> soundList;
// map<unsigned int, SfxrSound*> soundList;

void updateAndDrawFPS(int currentTime, v8::Isolate* isolate)
{
	static int frame = 0;
	static GLuint texId = 0;
	static int w = 0;
	static int h = 0;

	frame++;

	static int prec_time = 0;

	int deltatime = currentTime - prec_time;

	if (deltatime >= 1000)
	{
		cout << "foreground color: " << foregroundColor.r << ", " << foregroundColor.g << ", " << foregroundColor.b << endl;
		SDL_Color couleur = { foregroundColor.r, foregroundColor.g, foregroundColor.b, 255 };

		if (texId > 0) glDeleteTextures(1, &texId);
		wstringstream ss;
		ss << L"FPS: " << frame;
		texId = texteToTexture(font, couleur, ss.str(), w, h);
		prec_time = currentTime;
		frame = 0;
	}

	// Draw FPS texture
	if (texId > 0)
	{
		drawImage(texId, 22, 0, (float)w, (float)h, 1.0f);
	}

}

rgbColor webColorToRGB(const char* fillStyle)
{
	rgbColor rgb;
	string tmp(fillStyle);
	string r;
	string g;
	string b;

	if (strlen(fillStyle) == 4)
	{
		r = tmp.substr(1, 1) + tmp.substr(1, 1);
		g = tmp.substr(2, 1) + tmp.substr(2, 1);
		b = tmp.substr(3, 1) + tmp.substr(3, 1);
	}
	else
	{
		r = tmp.substr(1, 2);
		g = tmp.substr(3, 2);
		b = tmp.substr(5, 2);
	}

	stringstream r_ss;
	r_ss << std::hex << r;
	r_ss >> rgb.r;

	stringstream g_ss;
	g_ss << std::hex << g;
	g_ss >> rgb.g;

	stringstream b_ss;
	b_ss << std::hex << b;
	b_ss >> rgb.b;

	// cout << fillStyle << " : " << r << g << b << " : " << rgb.r << ", "<< rgb.g << ", "<< rgb.b <<endl;
	return rgb;
}

v8::Local<v8::Context> context;

void native_print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::String::Utf8Value str(isolate, args[0]);
	std::string cppStr(*str);

	cout << cppStr << endl;
}

void getWidth(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), width));
}

void getHeight(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), height));
}

void addNativeContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	int w = (args[0]->Int32Value(context)).FromMaybe(0);
	int h = (args[1]->Int32Value(context)).FromMaybe(0);

	sprites.push_back(new PuzzlescriptSprite(w, h));

	args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), sprites.size() - 1));
}

void nativeClearInstances(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	instances.erase(instances.begin(), instances.end());
}

void nativePlaySound(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if (sfxOn)
	{
		unsigned int seed = (args[0]->Int32Value(context)).FromMaybe(0);

		soundList[seed]->playSample();
	}
}

void nativeGenerateSound(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if (sfxOn)
	{
		unsigned int seed = (args[0]->Int32Value(context)).FromMaybe(0);
		if (soundList[seed] == nullptr)
		{
			SfxrSound* sfxrSound = new SfxrSound(
				// Envelope
				(float)(args[1]->NumberValue(context)).FromMaybe(0),
				(float)(args[2]->NumberValue(context)).FromMaybe(0),
				(float)(args[3]->NumberValue(context)).FromMaybe(0),
				(float)(args[4]->NumberValue(context)).FromMaybe(0),

				// Tone
				(float)(args[5]->NumberValue(context)).FromMaybe(0),
				(float)(args[6]->NumberValue(context)).FromMaybe(0),
				(float)(args[7]->NumberValue(context)).FromMaybe(0),
				(float)(args[8]->NumberValue(context)).FromMaybe(0),

				// Vibrato
				(float)(args[9]->NumberValue(context)).FromMaybe(0),
				(float)(args[10]->NumberValue(context)).FromMaybe(0),

				// Tonal change
				(float)(args[11]->NumberValue(context)).FromMaybe(0),
				(float)(args[12]->NumberValue(context)).FromMaybe(0),

				// Duty
				(float)(args[13]->NumberValue(context)).FromMaybe(0),
				(float)(args[14]->NumberValue(context)).FromMaybe(0),

				// Repeat
				(float)(args[15]->NumberValue(context)).FromMaybe(0),

				// Phaser
				(float)(args[16]->NumberValue(context)).FromMaybe(0),
				(float)(args[17]->NumberValue(context)).FromMaybe(0),

				// Low-pass filter
				(float)(args[18]->NumberValue(context)).FromMaybe(0),
				(float)(args[19]->NumberValue(context)).FromMaybe(0),
				(float)(args[20]->NumberValue(context)).FromMaybe(0),

				// High-pass filter
				(float)(args[21]->NumberValue(context)).FromMaybe(0),
				(float)(args[22]->NumberValue(context)).FromMaybe(0),

				// Main parameters
				(float)(args[23]->NumberValue(context)).FromMaybe(0),
				(args[24]->Int32Value(context)).FromMaybe(0),
				(args[25]->Int32Value(context)).FromMaybe(0),
				(args[26]->Int32Value(context)).FromMaybe(0)
			);

			soundList[seed] = new Sfx(to_string(seed), sfxrSound);
		}
	}
}

void nativeFillRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	int contextId = (args[0]->Int32Value(context)).FromMaybe(0);

	v8::Isolate* isolate = args.GetIsolate();
	v8::String::Utf8Value str(isolate, args[1]);
	std::string fillStyle(*str);

	int x = (args[2]->Int32Value(context)).FromMaybe(0);
	int y = (args[3]->Int32Value(context)).FromMaybe(0);
	int w = (args[4]->Int32Value(context)).FromMaybe(0);
	int h = (args[5]->Int32Value(context)).FromMaybe(0);

	if (fillStyle != string("transparent"))
	{
		SDL_Surface * s = sprites[contextId]->surface;
		SDL_Rect r;
		r.x = x;
		r.y = y;
		r.w = w;
		r.h = h;

		// convert web style color string to RGB
		rgbColor color = webColorToRGB(fillStyle.c_str());
		SDL_FillRect(s, &r, SDL_MapRGBA(s->format, color.r, color.g, color.b, 255));
	}
}

void nativeClearRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	int contextId = (args[0]->Int32Value(context)).FromMaybe(0);

	SDL_Surface * s = sprites[contextId]->surface;
	int x = (args[1]->Int32Value(context)).FromMaybe(0);
	int y = (args[2]->Int32Value(context)).FromMaybe(0);
	int w = (args[3]->Int32Value(context)).FromMaybe(0);
	int h = (args[4]->Int32Value(context)).FromMaybe(0);

	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	SDL_FillRect(s, &r, SDL_MapRGBA(s->format, 0, 0, 0, 0));
}

void drawImageNative(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	int imageId = (args[0]->Int32Value(context)).FromMaybe(0);
	int x = (args[1]->Int32Value(context)).FromMaybe(0);
	int y = (args[2]->Int32Value(context)).FromMaybe(0);

	spriteInstance si;

	si.id = imageId;
	si.x = x;
	si.y = y;

	instances.push_back(si);
}

v8::Local<v8::String> fileToString(string filename)
{
	std::ifstream in(filename);
	if (in) {
		ostringstream contents;
		contents << in.rdbuf();

		return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), contents.str().c_str());
	}

	return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "");
}

v8::Local<v8::Value> executeJavascriptFile(v8::Local<v8::Context> &context, string filename)
{
	// Create a string containing the JavaScript source code.
	v8::Local<v8::String> source = fileToString(filename);

	// Compile the source code.
	v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

	// Run the script to get the result.
	v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

	return result;
}

v8::Local<v8::Value> executeJavascript(v8::Local<v8::Context> &context, string src)
{
	// Create a string containing the JavaScript source code.
	v8::Local<v8::String> source = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), src.c_str());

	// Compile the source code.
	v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

	// Run the script to get the result.
	v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

	return result;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

string loadGame(string gameFile)
{
	ifstream t(gameFile);

	if (t.is_open())
	{
		string game;

		t.seekg(0, ios::end);
		game.reserve(t.tellg());
		t.seekg(0, ios::beg);

		game.assign((istreambuf_iterator<char>(t)),
			istreambuf_iterator<char>());
		stringstream ss;

		replaceAll(game, "\\", "\\\\");
		replaceAll(game, "\r", "");
		replaceAll(game, "\n", "\\n");
		replaceAll(game, "\"", "\\\"");

		ss << "var sourceCode = \"" << game << "\";\ncompile([\"restart\"],sourceCode);";

		return ss.str();
	}

	return "";
}

string getTitle(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "state.metadata.title");
	v8::String::Utf8Value gameTitle(isolate, result);

	return *gameTitle;
}

string getMusic(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "state.metadata.youtube");
	v8::String::Utf8Value gameMusic(isolate, result);

	return *gameMusic;
}

rgbColor getBackgroundColor(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "document.body.style.backgroundColor");
	v8::String::Utf8Value bg(isolate, result);
	rgbColor backgroundColor = webColorToRGB(*bg);

	return backgroundColor;
}

rgbColor getForegroundColor(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "state.fgcolor");
	v8::String::Utf8Value bg(isolate, result);
	rgbColor foregroundColor = webColorToRGB(*bg);

	return foregroundColor;
}

bool updateCellWidth(v8::Isolate* isolate)
{
	static int precCellWidth = 0;
	
	v8::Local<v8::Value> result = executeJavascript(context, "cellwidth");
	v8::String::Utf8Value cWidth(isolate, result);

	stringstream ss(*cWidth);
	ss >> cellWidth;

	bool change = (precCellWidth != cellWidth);

	precCellWidth = cellWidth;
	return change;
}

bool updateCellHeight(v8::Isolate* isolate)
{
	static int precCellHeight = 0;

	v8::Local<v8::Value> result = executeJavascript(context, "cellheight");
	v8::String::Utf8Value cHeight(isolate, result);

	stringstream ss(*cHeight);
	ss >> cellHeight;

	bool change = (precCellHeight != cellHeight);

	precCellHeight = cellHeight;
	return change;
}

void updateDeltaTime(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "deltatime");
	v8::String::Utf8Value delta(isolate, result);

	stringstream ss(*delta);
	ss >> puzzlescriptInterval;
}

bool getQuittingTitleScreen(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "quittingTitleScreen");
	v8::String::Utf8Value quittingTitleScreen(isolate, result);

	return (*quittingTitleScreen == string("true") ? true : false);
}

bool getIsTitleScreen(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "titleScreen");
	v8::String::Utf8Value isTitleScreen(isolate, result);
	
	return (*isTitleScreen == string("true") ? true : false);
}

void updatePuzzlescript(v8::Isolate* isolate, int dt)
{
	static int cumulativeTime = 0;
	cumulativeTime += dt;
	if (cumulativeTime >= puzzlescriptInterval)
	{
		executeJavascript(context, "update()");
		bool change = updateCellWidth(isolate);
		change = change | updateCellHeight(isolate);

		// Force redrawing sprites if game size changed
		if (change)
		{
			for (int i = 0; i < sprites.size(); i++)
			{
				sprites[i]->init(cellWidth, cellHeight);
			}
			executeJavascript(context, "forceRegenImages=true;");
			executeJavascript(context, "canvasResize()");
		}
		cumulativeTime = 0;
	}
}

string getLocalStorage(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "localStorage.serialize()");
	v8::String::Utf8Value localStorageData(isolate, result);

	return *localStorageData;
}

void saveGameState(string localStorageFile, v8::Isolate* isolate)
{
	string localStorageData = getLocalStorage(isolate);
	// Save to file
	ofstream storage(localStorageFile);
	if (storage)
	{
		storage << localStorageData << endl;
		storage.close();
	}
}

void loadGameState(string localStorageFile)
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

		executeJavascript(context, localStorageData.c_str());
		storage.close();
	}
	else
	{
		executeJavascript(context, "localStorage.MyGame = 0;");
	}
}

void freeGame()
{
	instances.clear();

	for (std::vector<PuzzlescriptSprite*>::iterator it = sprites.begin(); it != sprites.end(); ++it)
	{
		delete *it;
	}

	sprites.clear();
}
void initGame(string gameFile, v8::Isolate* isolate, string jsPath, string puzzlescriptPath)
{
	freeGame();

	executeJavascriptFile(context, jsPath + "navigator.js");
	executeJavascriptFile(context, jsPath + "window.js");

	loadGameState("./data/localStorage/" + gameFile);

	executeJavascriptFile(context, jsPath + "document.js");
	executeJavascriptFile(context, puzzlescriptPath + "storagewrapper.js");
	executeJavascriptFile(context, puzzlescriptPath + "globalVariables.js");
	executeJavascriptFile(context, puzzlescriptPath + "debug_off.js");
	executeJavascriptFile(context, puzzlescriptPath + "font.js");
	executeJavascriptFile(context, puzzlescriptPath + "rng.js");
	executeJavascriptFile(context, puzzlescriptPath + "riffwave.js");
	executeJavascriptFile(context, puzzlescriptPath + "sfxr.js");

	executeJavascriptFile(context, jsPath + "sfxr.js");
	executeJavascriptFile(context, jsPath + "codemirror.js");

	executeJavascriptFile(context, puzzlescriptPath + "colors.js");
	executeJavascriptFile(context, puzzlescriptPath + "graphics.js");
	executeJavascriptFile(context, puzzlescriptPath + "engine.js");
	executeJavascriptFile(context, puzzlescriptPath + "parser.js");
	executeJavascriptFile(context, puzzlescriptPath + "compiler.js");
	executeJavascriptFile(context, puzzlescriptPath + "inputoutput.js");

	executeJavascriptFile(context, jsPath + "overload.js");

	executeJavascript(context, loadGame("./data/games/" + gameFile));

	gameTitle = getTitle(isolate);
	string gameMusic = getMusic(isolate);
	backgroundColor = getBackgroundColor(isolate);
	foregroundColor = getForegroundColor(isolate);
	updateCellWidth(isolate);
	updateCellHeight(isolate);

	glClearColor((float)backgroundColor.r / 255.0, (float)backgroundColor.g / 255.0, (float)backgroundColor.b / 255.0, 1.0);
	SDL_SetWindowTitle(window, gameTitle.c_str());

	// INIT GAME MUSIC

	if (music != nullptr)
	{
		Mix_FreeMusic(music);
		music = nullptr;
	}

	string supportedFormatList[] = { "mp3", "ogg", "wav", "aiff", "voc","mod", "xm", "s3m", "669", "it", "med", "mid" };
	for (int i = 0; i < 12; i++)
	{
		stringstream ss;
		ss << "./data/music/" << gameMusic << "." << supportedFormatList[i];
		music = Mix_LoadMUS(ss.str().c_str());

		if (music != nullptr)
		{
			Mix_PlayMusic(music, -1);
			Mix_VolumeMusic(getMusicVolume());
			break;
		}
	}
}

// CONTROLLERS MANAGEMENT

map<int, SDL_GameController *> controllers; ///< Liste des controllers branch�s

int addController(int id)
{
	if (SDL_IsGameController(id)) {
		SDL_GameController *pad = SDL_GameControllerOpen(id);

		if (pad)
		{
			SDL_Joystick *joy = SDL_GameControllerGetJoystick(pad);
			int instanceID = SDL_JoystickInstanceID(joy);

			controllers[instanceID] = pad;
			cout << "Branchement du controller " << instanceID << endl;
			return instanceID;
		}
	}
	return -1;
}

void removeController(int id)
{
	cout << "Debranchement du controller " << id << endl;
	SDL_GameController *pad = controllers[id];
	SDL_GameControllerClose(pad);
	controllers[id] = NULL;
	controllers.erase(id);
}

void foundControllers()
{
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		addController(i);
	}
}

void windowResize(int w, int h, v8::Isolate* isolate)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	glLoadIdentity();

	glOrtho(0, width, height, 0, -1, 1);

	stringstream windowChange;
	windowChange << "canvas.parentNode.clientWidth = " << width << ";\n";
	windowChange << "canvas.parentNode.clientHeight = " << height << ";";
	executeJavascript(context, windowChange.str());

	updateCellWidth(isolate);
	updateCellHeight(isolate);

	for (int i = 0; i < sprites.size(); i++)
	{
		sprites[i]->init(cellWidth, cellHeight);
	}
	executeJavascript(context, "forceRegenImages=true;");
	executeJavascript(context, "canvasResize()");
}

void toggleFullscreen(v8::Isolate* isolate)
{
	if (fullscreen)
	{
		SDL_SetWindowFullscreen(window, 0);
		cout << width << ", " << height << endl;
		windowResize(windowedWidth, windowedHeight, isolate);
		fullscreen = false;
	}
	else
	{
		windowedWidth = width;
		windowedHeight = height;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		fullscreen = true;
	}
}

// MAIN
int main(int argc, char* argv[])
{
	// Choose normal or minified puzzlescript source
	#ifdef __USE_MINIFIED__
		string puzzlescriptPath("data/js/puzzlescript/minified/");
	#else
		string puzzlescriptPath("data/js/puzzlescript/js/");
	#endif

	bool showGameList = true;
	bool allowGameList = true;

	string gameFile = "";

	Config config("./config.json");
	width = config.data["width"];
	height = config.data["height"];
	sfxOn = config.data["sfx"];
	musicOn = config.data["music"];
	music_volume_index = config.data["musicVolume"];
	sfxr_volume_index = config.data["sfxVolume"];
	chunk_volume_index = config.data["sfxVolume"];
	fullscreen = config.data["fullscreen"];

	sfxr_master_vol = sfxr_volume_scale[sfxr_volume_index];

	windowedWidth = width;
	windowedHeight = height;

	if (argc > 1)
	{
		string arg = string(argv[1]);
		if (arg == "/v" || arg == "-v" || arg == "--version")
		{
			cout << __PS_WRAPPER_VERSION__ << endl;
			return 0;
		}
		else
		{
			gameFile = arg;
			showGameList = false;
		}
	}

	string jsPath("data/js/");

	// Initialize V8.
	v8::V8::InitializeICUDefaultLocation(argv[0]);
	v8::V8::InitializeExternalStartupData(argv[0]);
	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	// Create a new Isolate and make it the current one.
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	v8::Isolate* isolate = v8::Isolate::New(create_params);


	v8::Isolate::Scope isolate_scope(isolate);
	// Create a stack-allocated handle scope.
	v8::HandleScope handle_scope(isolate);

	// Create a new context.
	context = v8::Context::New(isolate);
	// Enter the context for compiling and running the hello world script.
	v8::Context::Scope context_scope(context);

	// ADD C++ FUNCTIONS IN JAVASCRIPT CONTEXT
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "print"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), native_print)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "getWidth"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), getWidth)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "getHeight"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), getHeight)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "addNativeContext"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), addNativeContext)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "nativeClearInstances"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), nativeClearInstances)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "nativePlaySound"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), nativePlaySound)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "nativeGenerateSound"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), nativeGenerateSound)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "nativeFillRect"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), nativeFillRect)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "nativeClearRect"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), nativeClearRect)->GetFunction());
	context->Global()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "drawImageNative"), v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), drawImageNative)->GetFunction());

	SDL_GLContext glContext;
	SDL_Event event;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(1);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
	{
		string msg = "SDL failed to initialize: ";
		msg.append(SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
		return 0;
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
	{
		printf("Error initializing SDL_mixer: %s\n", Mix_GetError());
		exit(1);
	}

	int nbMappingsAdded = SDL_GameControllerAddMappingsFromFile("./data/gamecontrollerdb.txt");
	if (nbMappingsAdded == -1)
	{
		cout << SDL_GetError() << endl;
	}
	else
	{
		cout << "Nombre de mappings ajoutes : " << nbMappingsAdded << endl;
	}

	foundControllers();

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	if (fullscreen) flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
	window = SDL_CreateWindow(gameTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

	if (window == nullptr)
	{
		std::string msg = "Window could not be created: ";
		msg.append(SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
		return 0;
	}

	glContext = SDL_GL_CreateContext(window);

	if (glContext == nullptr)
	{
		std::string msg = "OpenGL context could not be created: ";
		msg.append(SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
		return 0;
	}

	initSfxr();

	font = loadFont("./data/fonts/OxygenMono-Regular.ttf");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor((float)backgroundColor.r / 255.0, (float)backgroundColor.g / 255.0, (float)backgroundColor.b / 255.0, 1.0);
	glViewport(0, 0, width, height);
	glOrtho(0, width, height, 0, -1, 1);

	// Disable vsync for performance purpose
	SDL_GL_SetSwapInterval(0);

	glewInit();

	// INIT GAME LIST
	GUI_List gameList(20, 120, 28, 200, 20, font, { 255 , 255 , 255, 255 });
	gameList.initFromDirectory("./data/games/");

	allowGameList = (gameList.entries.size() > 1);

	if (gameFile != "")
	{
		if (!gameList.entryExist(gameFile))
		{
			stringstream ss;
			ss << "The specified game file does not exist: " << gameFile;
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
				"Missing game file",
				ss.str().c_str(),
				NULL);

			gameFile = "";
		}
	}

	if (gameFile == "")
	{
		if (gameList.entryExist("game")) gameFile = "game";
		if (gameList.entryExist("Game")) gameFile = "Game";
		if (gameList.entryExist("GAME")) gameFile = "GAME";
		if (gameList.entryExist("default")) gameFile = "default";
		if (gameList.entryExist("Default")) gameFile = "Default";
		if (gameList.entryExist("DEFAULT")) gameFile = "DEFAULT";

		if (gameList.entries.size() == 1) gameFile = gameList.entries[0]->valeur;
	}

	initGame(gameFile, isolate, jsPath, puzzlescriptPath);

	SDL_Color fgc = { foregroundColor.r, foregroundColor.g, foregroundColor.b, 255 };
	gameList.setColor(fgc);

	showGameList = (gameFile != "") ? false : allowGameList;

	GUI_Button* toggleFullscreenButton = new GUI_Button("./data/images/togglefullscreen.png", 0, 0, 32, 18, 4);
	GUI_Button* quitButton = new GUI_Button("./data/images/btquit.png", 0, 0, 18, 18, 2);
	GUI_Button* toggleGameListButton = new GUI_Button("./data/images/btgamelist.png", 0, 0, 18, 18, 2);

	GLuint gaugeButton = loadImage("./data/images/btgauge.png");
	GLuint gaugeButtonZero = loadImage("./data/images/btgaugezero.png");
	DiscreetGauge musicGauge(music_volume_index, gaugeButton, gaugeButtonZero, 10.0f, 30.0f, 19.0, 18.0, 17, font, L"Music", foregroundColor);
	DiscreetGauge soundGauge(chunk_volume_index, gaugeButton, gaugeButtonZero, 10.0f, 70.0f, 19.0, 18.0, 17, font, L"Sfx", foregroundColor);
	
	// INIT MAIN LOOP
	bool running = true;
	int currentTime = SDL_GetTicks();
	int precTime = currentTime;
	int deltaTime = 0;

	int mouse_x = 0;
	int mouse_y = 0;

	updateDeltaTime(isolate);

	// MAIN LOOP
	while (running)
	{
		// TIME
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - precTime;
		precTime = currentTime;

		showMouseHud = max(0, showMouseHud - deltaTime);

		// MOUSE
		SDL_GetMouseState(&mouse_x, &mouse_y);

		// PUZZLESCRIPT UPDATE
		updatePuzzlescript(isolate, deltaTime);

		// INPUTS
		SDL_Keymod keyMod = SDL_GetModState();

		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
				case SDL_CONTROLLERDEVICEADDED:
					addController(event.cdevice.which);
					break;
				case SDL_CONTROLLERDEVICEREMOVED:
					removeController(event.cdevice.which);
					break;
				case SDL_CONTROLLERBUTTONDOWN:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_START:
							executeJavascript(context, "onKeyDown({keyCode: 82})");
							break;
						case SDL_CONTROLLER_BUTTON_A:
							executeJavascript(context, "onKeyDown({keyCode: 13})");
							break;
						case SDL_CONTROLLER_BUTTON_B:
							executeJavascript(context, "onKeyDown({keyCode: 85})");
							break;
						case SDL_CONTROLLER_BUTTON_BACK:
							if (!getIsTitleScreen(isolate))
							{
								executeJavascript(context, "onKeyDown({keyCode: 27})");
							}
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							executeJavascript(context, "onKeyDown({keyCode: 37})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							executeJavascript(context, "onKeyDown({keyCode: 39})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_UP:
							executeJavascript(context, "onKeyDown({keyCode: 38})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							executeJavascript(context, "onKeyDown({keyCode: 40})");
							break;
						default:
							cout << (int)(event.cbutton.button) << endl;
					}
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_START:
							executeJavascript(context, "onKeyUp({keyCode: 82})");
							break;
						case SDL_CONTROLLER_BUTTON_A:
							executeJavascript(context, "onKeyUp({keyCode: 13})");
							break;
						case SDL_CONTROLLER_BUTTON_B:
							executeJavascript(context, "onKeyUp({keyCode: 85})");
							break;
						case SDL_CONTROLLER_BUTTON_BACK:
							executeJavascript(context, "onKeyUp({keyCode: 27})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							executeJavascript(context, "onKeyUp({keyCode: 37})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							executeJavascript(context, "onKeyUp({keyCode: 39})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_UP:
							executeJavascript(context, "onKeyUp({keyCode: 38})");
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							executeJavascript(context, "onKeyUp({keyCode: 40})");
							break;
						default:
							cout << (int)(event.cbutton.button) << endl;
					}
					break;
				case SDL_MOUSEMOTION:
					showMouseHud = MouseHudDelay;
					break;
				case SDL_MOUSEBUTTONDOWN:
					showMouseHud = MouseHudDelay;
					break;
				case SDL_MOUSEBUTTONUP:
					showMouseHud = MouseHudDelay;

					if (showGameList && gameList.selectedEntry)
					{
						if (gameFile != gameList.selectedEntry->valeur)
						{
							if (gameFile != "" && gameList.entryExist(gameFile))
							{
								saveGameState("./data/localStorage/" + gameFile, isolate);
							}

							gameFile = gameList.selectedEntry->valeur;
							initGame(gameFile, isolate, jsPath, puzzlescriptPath);

							musicGauge.updateTextColor(foregroundColor);
							soundGauge.updateTextColor(foregroundColor);
							SDL_Color fgc = { foregroundColor.r, foregroundColor.g, foregroundColor.b, 255 };
							gameList.setColor(fgc);
							cout << gameList.selectedEntry->valeur << endl;
						}

						showGameList = false;
					}

					if (toggleFullscreenButton->isIn(mouse_x, mouse_y))
					{
						toggleFullscreen(isolate);
					}

					if (quitButton->isIn(mouse_x, mouse_y))
					{
						running = false;
					}

					if (allowGameList && toggleGameListButton->isIn(mouse_x, mouse_y))
					{
						showGameList = !showGameList;
					}

					{
						int newMusicVolume = musicGauge.onClick(mouse_x, mouse_y);
						if (newMusicVolume >= 0 && newMusicVolume != music_volume_index)
						{
							music_volume_index = newMusicVolume;
							Mix_VolumeMusic(getMusicVolume());
						}
					}

					{
						int newSoundVolume = soundGauge.onClick(mouse_x, mouse_y);
						if (newSoundVolume >= 0 && newSoundVolume != chunk_volume_index)
						{
							chunk_volume_index = newSoundVolume;

							sfxr_volume_index = newSoundVolume;
							sfxr_master_vol = sfxr_volume_scale[sfxr_volume_index];
						}
					}

					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						windowResize(event.window.data1, event.window.data2, isolate);
					}
					break;
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_RETURN)
					{
						if (keyMod & KMOD_ALT)
						{
							toggleFullscreen(isolate);
						}
					}

					if (event.key.keysym.sym == SDLK_o)
					{
						sfxr_volume_index = min(16, sfxr_volume_index + 1);
						sfxr_master_vol = sfxr_volume_scale[sfxr_volume_index];

						chunk_volume_index = min(16, chunk_volume_index + 1);
					}

					if (event.key.keysym.sym == SDLK_l)
					{
						sfxr_volume_index = max(0, sfxr_volume_index - 1);
						sfxr_master_vol = sfxr_volume_scale[sfxr_volume_index];

						chunk_volume_index = max(0, chunk_volume_index - 1);
					}

					if (event.key.keysym.sym == SDLK_p)
					{
						music_volume_index = min(16, music_volume_index + 1);
						Mix_VolumeMusic(getMusicVolume());
					}

					if (event.key.keysym.sym == SDLK_m)
					{
						music_volume_index = max(0, music_volume_index - 1);
						Mix_VolumeMusic(getMusicVolume());
					}

					if ((event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN
						|| event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_x) && !(keyMod & KMOD_ALT))
					{
						executeJavascript(context, "onKeyDown({keyCode: 13})");
					}

					if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
					{
						executeJavascript(context, "onKeyDown({keyCode: 37})");
					}

					if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
					{
						executeJavascript(context, "onKeyDown({keyCode: 39})");
					}
					if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
					{
						executeJavascript(context, "onKeyDown({keyCode: 38})");
					}

					if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
					{
						executeJavascript(context, "onKeyDown({keyCode: 40})");
					}
					if (event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_z)
					{
						executeJavascript(context, "onKeyDown({keyCode: 85})");
					}
					if (event.key.keysym.sym == SDLK_r)
					{
						executeJavascript(context, "onKeyDown({keyCode: 82})");
					}
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						if (!getIsTitleScreen(isolate))
						{
							executeJavascript(context, "onKeyDown({keyCode: 27})");
						}
					}
					if (event.key.keysym.sym == SDLK_e)
					{
						executeJavascript(context, "onKeyDown({keyCode: 69})");
					}
					if (event.key.keysym.sym == SDLK_0)
					{
						executeJavascript(context, "onKeyDown({keyCode: 48})");
					}
					if (event.key.keysym.sym == SDLK_1)
					{
						executeJavascript(context, "onKeyDown({keyCode: 49})");
					}
					if (event.key.keysym.sym == SDLK_2)
					{
						executeJavascript(context, "onKeyDown({keyCode: 50})");
					}
					if (event.key.keysym.sym == SDLK_3)
					{
						executeJavascript(context, "onKeyDown({keyCode: 51})");
					}
					if (event.key.keysym.sym == SDLK_4)
					{
						executeJavascript(context, "onKeyDown({keyCode: 52})");
					}
					if (event.key.keysym.sym == SDLK_5)
					{
						executeJavascript(context, "onKeyDown({keyCode: 53})");
					}
					if (event.key.keysym.sym == SDLK_6)
					{
						executeJavascript(context, "onKeyDown({keyCode: 54})");
					}
					if (event.key.keysym.sym == SDLK_7)
					{
						executeJavascript(context, "onKeyDown({keyCode: 55})");
					}
					if (event.key.keysym.sym == SDLK_8)
					{
						executeJavascript(context, "onKeyDown({keyCode: 56})");
					}
					if (event.key.keysym.sym == SDLK_9)
					{
						executeJavascript(context, "onKeyDown({keyCode: 57})");
					}

					if (event.key.keysym.sym == SDLK_TAB)
					{
						if (allowGameList)
						{
							showGameList = !showGameList;
						}
					}

					if (event.key.keysym.sym == SDLK_i)
					{
						showFPS = !showFPS;
					}

					break;
				case SDL_KEYUP:
					if ((event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN
						|| event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_x) && !(keyMod & KMOD_ALT))
					{
						executeJavascript(context, "onKeyUp({keyCode: 13})");
					}

					if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
					{
						executeJavascript(context, "onKeyUp({keyCode: 37})");
					}

					if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
					{
						executeJavascript(context, "onKeyUp({keyCode: 39})");
					}
					if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
					{
						executeJavascript(context, "onKeyUp({keyCode: 38})");
					}

					if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
					{
						executeJavascript(context, "onKeyUp({keyCode: 40})");
					}
					if (event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_z)
					{
						executeJavascript(context, "onKeyUp({keyCode: 85})");
					}
					if (event.key.keysym.sym == SDLK_r)
					{
						executeJavascript(context, "onKeyUp({keyCode: 82})");
					}
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						executeJavascript(context, "onKeyUp({keyCode: 27})");
					}
					if (event.key.keysym.sym == SDLK_e)
					{
						executeJavascript(context, "onKeyUp({keyCode: 69})");
					}
					if (event.key.keysym.sym == SDLK_0)
					{
						executeJavascript(context, "onKeyUp({keyCode: 48})");
					}
					if (event.key.keysym.sym == SDLK_1)
					{
						executeJavascript(context, "onKeyUp({keyCode: 49})");
					}
					if (event.key.keysym.sym == SDLK_2)
					{
						executeJavascript(context, "onKeyUp({keyCode: 50})");
					}
					if (event.key.keysym.sym == SDLK_3)
					{
						executeJavascript(context, "onKeyUp({keyCode: 51})");
					}
					if (event.key.keysym.sym == SDLK_4)
					{
						executeJavascript(context, "onKeyUp({keyCode: 52})");
					}
					if (event.key.keysym.sym == SDLK_5)
					{
						executeJavascript(context, "onKeyUp({keyCode: 53})");
					}
					if (event.key.keysym.sym == SDLK_6)
					{
						executeJavascript(context, "onKeyUp({keyCode: 54})");
					}
					if (event.key.keysym.sym == SDLK_7)
					{
						executeJavascript(context, "onKeyUp({keyCode: 55})");
					}
					if (event.key.keysym.sym == SDLK_8)
					{
						executeJavascript(context, "onKeyUp({keyCode: 56})");
					}
					if (event.key.keysym.sym == SDLK_9)
					{
						executeJavascript(context, "onKeyUp({keyCode: 57})");
					}
					break;
			}
		}

		// DISPLAY
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (showGameList)
		{
			// cout << mouse_x << ", " << mouse_y << endl;
			gameList.setSelectedEntry(mouse_x, mouse_y);
			gameList.draw(mouse_x, mouse_y);
		}
		else
		{
			for (int i = 0; i < instances.size(); i++)
			{
				sprites[instances[i].id]->draw(instances[i].x, instances[i].y, cellWidth, cellHeight);
			}
		}

		// Display GUI
		if (showMouseHud > 0)
		{
			toggleFullscreenButton->setPos(width - 32 - 22, 0);
			if (toggleFullscreenButton->isIn(mouse_x, mouse_y))
			{
				toggleFullscreenButton->setEtat(fullscreen ? 3 : 2);
			}
			else
			{
				toggleFullscreenButton->setEtat(fullscreen ? 1 : 0);
			}
			toggleFullscreenButton->draw();

			quitButton->setPos(width - 18, 0);
			quitButton->setEtat(quitButton->isIn(mouse_x, mouse_y) ? 1 : 0);
			quitButton->draw();

			if (allowGameList)
			{
				toggleGameListButton->setEtat(toggleGameListButton->isIn(mouse_x, mouse_y) ? 1 : 0);
				toggleGameListButton->draw();
			}

			musicGauge.draw();
			soundGauge.draw();
		}

		// Display all sprites models
		//for (int i = 0; i < sprites.size(); i++)
		// {
		// 	sprites[i]->draw(i % 34 * 30, i / 34 * 30, 30, 30);
		//}

		// display framerate
		if (showFPS) updateAndDrawFPS(currentTime, isolate);

		SDL_GL_SwapWindow(window);
	}

	if (gameFile != "" && gameList.entryExist(gameFile))
	{
		saveGameState("./data/localStorage/" + gameFile, isolate);
	}

	freeGame();

	SDL_GL_DeleteContext(glContext);
	glContext = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;
	Mix_CloseAudio();
	SDL_Quit();

	// Shutdown V8 javascript engine.
	v8::V8::ShutdownPlatform();
	delete create_params.array_buffer_allocator;

	return 0;
}
