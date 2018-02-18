/* ***********************************************************************************
 * Copyright (C) 2015-2018 Xavier Direz - http://www.LaFaceObscureDuPixel.fr         *
 *                                                                                   *
 * This file is part of ObscureGameEngine.                                           *
 *                                                                                   *
 * ObscureGameEngine is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU Lesser General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * ObscureGameEngine is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU Lesser General Public License for more details.                               *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public License          *
 * along with  this program; If not, see <http://www.gnu.org/licenses/>.             *
 *************************************************************************************/
 
#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include <iostream>
#include <vector>
// #include <irrKlang.h>

#include "engine.hpp"

using namespace std;
// using namespace irrklang;

class Screen;

class Application
{
	public:
	Engine* engine;

	// ISoundSource* music;
    // ISound* musicSound;
    // string currentMusicFile;

	// ISoundEngine* sound_engine;

	vector<Screen*> screenList;
	int currentScreen;

	Application(Engine* _engine);
	~Application();

	void mainLoop(); ///< Boucle principale de l'application

	Screen* setScreen(int _id);
	Screen* setScreen(string _id);
	Screen* getScreen(string _id);
	Screen* getCurrentScreen();

	// void setMusic(string musicFile);
};

class Screen
{
	public:

	string id;
	Application* app;

	Screen(Application* _app, string _id);
	~Screen();

	virtual void update()=0;///< Mise à jour des éléments de l'écran (gestion de la physique, des inputs, IA, règles du jeu...)
	virtual void draw()=0;  ///< Affichage des éléments de l'écran
	virtual void eventManagement();
	/**
        Fonction appelée à chaque fois qu'on entre dans cet écran
        @param id_orig : id de l'écran dont on vient
	*/
    virtual void onSet(string id_orig)=0;
	/**
        Fonction appelée à chaque fois qu'on sort de cet écran
        @param id_dest : id de l'écran que l'on va afficher
	*/
    virtual void onQuit(string id_dest)=0;


	virtual void event_AUDIODEVICEADDED(SDL_Event &event);
    virtual void event_AUDIODEVICEREMOVED(SDL_Event &event);
    virtual void event_CONTROLLERAXISMOTION(SDL_Event &event);
    virtual void event_CONTROLLERBUTTONDOWN(SDL_Event &event);
    virtual void event_CONTROLLERBUTTONUP(SDL_Event &event);
    virtual void event_CONTROLLERDEVICEADDED(SDL_Event &event);
    virtual void event_CONTROLLERDEVICEREMOVED(SDL_Event &event);
    virtual void event_CONTROLLERDEVICEREMAPPED(SDL_Event &event);
    virtual void event_DOLLARGESTURE(SDL_Event &event);
    virtual void event_DOLLARRECORD(SDL_Event &event);
    virtual void event_DROPFILE(SDL_Event &event);
    virtual void event_DROPTEXT(SDL_Event &event);
    virtual void event_DROPBEGIN(SDL_Event &event);
    virtual void event_DROPCOMPLETE(SDL_Event &event);
    virtual void event_FINGERMOTION(SDL_Event &event);
    virtual void event_FINGERDOWN(SDL_Event &event);
    virtual void event_FINGERUP(SDL_Event &event);
    virtual void event_KEYDOWN(SDL_Event &event);
    virtual void event_KEYUP(SDL_Event &event);
    virtual void event_JOYAXISMOTION(SDL_Event &event);
    virtual void event_JOYBALLMOTION(SDL_Event &event);
    virtual void event_JOYHATMOTION(SDL_Event &event);
    virtual void event_JOYBUTTONDOWN(SDL_Event &event);
    virtual void event_JOYBUTTONUP(SDL_Event &event);
    virtual void event_JOYDEVICEADDED(SDL_Event &event);
    virtual void event_JOYDEVICEREMOVED(SDL_Event &event);
    virtual void event_MOUSEMOTION(SDL_Event &event);
    virtual void event_MOUSEBUTTONDOWN(SDL_Event &event);
    virtual void event_MOUSEBUTTONUP(SDL_Event &event);
    virtual void event_MOUSEWHEEL(SDL_Event &event);
    virtual void event_MULTIGESTURE(SDL_Event &event);
    virtual void event_QUIT(SDL_Event &event);
    virtual void event_SYSWMEVENT(SDL_Event &event);
    virtual void event_TEXTEDITING(SDL_Event &event);
    virtual void event_TEXTINPUT(SDL_Event &event);
    virtual void event_USEREVENT(SDL_Event &event);
    virtual void event_WINDOWEVENT(SDL_Event &event);
};

#endif
