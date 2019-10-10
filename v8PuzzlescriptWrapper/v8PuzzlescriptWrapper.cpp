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
#define __PS_WRAPPER_VERSION__ "v2.0.0"
#endif

// #define __USE_MINIFIED__
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/glew.h>
#include <v8.h>
#include <libplatform/libplatform.h>
#include "sfxr.hpp"
#include "gui.hpp"
#include "gameEngine/json.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

using namespace std;

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

void updateAndDrawFPS(int currentTime)
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
		SDL_Color couleur = { 255 , 255 , 255, 255 };

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
		drawImage(texId, 4, 0, (float)w, (float)h, 1.0f);
	}

}

GLuint SurfaceToTexture(SDL_Surface *surface)
{
	//return createRedTexture();
	GLuint tid;
	GLenum texture_format;
	GLint ncolors;
	SDL_Surface* s = surface;

	/* Convert SDL_Surface to OpenGL Texture */
	ncolors = s->format->BytesPerPixel;
	if (ncolors == 4) {
		//alpha channel
		if (s->format->Rmask == 0x000000ff)
		{
			texture_format = GL_RGBA;
		}
		else
		{
			texture_format = GL_BGRA;
		}
	}
	else {
		if (ncolors == 3) {
			//no alpha channel
			if (s->format->Rmask == 0x0000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}
		else {
			return 0;
		}
	}

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, ncolors, s->w, s->h, 0,
		texture_format, GL_UNSIGNED_BYTE, s->pixels);

	return tid;
}


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

vector<PuzzlescriptSprite*> sprites;
vector<spriteInstance> instances;

int width = 1024;
int height = 768;
int windowedWidth = 1024;
int windowedHeight = 768;
bool fullscreen = false;

int cellWidth;
int cellHeight;
int puzzlescriptInterval;
rgbColor backgroundColor;
string gameTitle;

bool sfxOn=true;
bool showFPS = false;

map<unsigned int, SfxrSound*> soundList;

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

		soundList[seed] = new SfxrSound(
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

rgbColor getBackgroundColor(v8::Isolate* isolate)
{
	v8::Local<v8::Value> result = executeJavascript(context, "document.body.style.backgroundColor");
	v8::String::Utf8Value bg(isolate, result);
	rgbColor backgroundColor = webColorToRGB(*bg);

	return backgroundColor;
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
	backgroundColor = getBackgroundColor(isolate);
	updateCellWidth(isolate);
	updateCellHeight(isolate);

	glClearColor((float)backgroundColor.r / 255.0, (float)backgroundColor.g / 255.0, (float)backgroundColor.b / 255.0, 1.0);
}

// CONTROLLERS MANAGEMENT

map<int, SDL_GameController *> controllers; ///< Liste des controllers branchés

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
	fullscreen = config.data["fullscreen"];

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

	SDL_Window* window;
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
	GUI_List gameList(20, 20, 28, 200, 24, font, { 255 , 255 , 255, 255 });
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

	showGameList = (gameFile != "") ? false : allowGameList;

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
							if (getIsTitleScreen(isolate))
							{
								running = false;
							}
							else
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
				case SDL_MOUSEBUTTONUP:
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
							cout << gameList.selectedEntry->valeur << endl;
						}

						showGameList = false;
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
						if (getIsTitleScreen(isolate))
						{
							running = false;
						}
						else
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

		// Display all sprites models
		//for (int i = 0; i < sprites.size(); i++)
		// {
		// 	sprites[i]->draw(i % 34 * 30, i / 34 * 30, 30, 30);
		//}

		// display framerate
		if (showFPS) updateAndDrawFPS(currentTime);

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
	SDL_Quit();

	// Shutdown V8 javascript engine.
	v8::V8::ShutdownPlatform();
	delete create_params.array_buffer_allocator;

	return 0;
}
