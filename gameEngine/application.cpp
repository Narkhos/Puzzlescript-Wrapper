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
 
#include "application.hpp"

// class Application

Application::Application(Engine* _engine)
	:engine(_engine),currentScreen(0)
{
	// music=NULL;
    // musicSound = NULL;
    // currentMusicFile = "";

	// sound_engine = createIrrKlangDevice();
}

Application::~Application()
{

}

void Application::mainLoop()
{
	while(engine->running)
    {
        // ACTUALISATION DU CONTEXT
        engine->update();
		if(currentScreen>=0)
		{
			if(screenList[currentScreen] != NULL)
			{
				screenList[currentScreen]->update();

				if(currentScreen >= 0)
                {
                    screenList[currentScreen]->eventManagement();

                    glClear(GL_COLOR_BUFFER_BIT);

                    screenList[currentScreen]->draw();

                    glFlush();
                    SDL_GL_SwapWindow(engine->display->screen);
                }
			}
			else
			{
				cout << "L'ecran " << currentScreen << " n'a pas ete trouvé" << endl;
				engine->running = false;
			}
		}
		else
		{
			engine->running = false;
		}

	}
}

Screen* Application::setScreen(string _id)
{
	for(int i = 0; i < screenList.size(); ++i)
	{
		if(screenList[i] != NULL && screenList[i]->id == _id)
		{
            Screen* tmp = this->getCurrentScreen();
            string id_orig = "";
            if(tmp!=NULL)
            {
                tmp->onQuit(_id);
                id_orig = tmp->id;
            }

			this->currentScreen = i;
			screenList[i]->onSet(id_orig);
			return screenList[i];
		}
	}
	return NULL;
}

Screen* Application::setScreen(int _id)
{
    if(_id >= 0 && _id < screenList.size())
    {
        Screen* tmp = this->getCurrentScreen();
        string id_orig = "";
        if(tmp!=NULL)
        {
            tmp->onQuit(screenList[_id]->id);
            id_orig = tmp->id;
        }

        currentScreen = _id;
        screenList[currentScreen]->onSet(id_orig);
        return screenList[currentScreen];
    }
    else
    {
        currentScreen = -1;
        return NULL;
    }
}

Screen* Application::getScreen(string _id)
{
	for(int i = 0; i < screenList.size(); ++i)
	{
		if(screenList[i] != NULL && screenList[i]->id == _id)
		{
			return screenList[i];
		}
	}
	return NULL;
}

Screen* Application::getCurrentScreen()
{
	if(currentScreen>=0 && currentScreen < screenList.size())
	{
		return screenList[currentScreen];
	}
	else
	{
		return NULL;
	}
}

/*void Application::setMusic(string musicFile)
{
    if(this->currentMusicFile != musicFile || this->music == NULL)
    {
        this->currentMusicFile = musicFile;
        if(this->music) sound_engine->removeSoundSource(music);
        this->music = sound_engine->addSoundSourceFromFile(this->currentMusicFile.c_str());

        if(this->music)
        {
            musicSound = sound_engine->play2D(music, true, false, false, true);
        }
    }
}
*/

// class Screen

Screen::Screen(Application* _app, string _id)
	:app(_app), id(_id)
{

}

Screen::~Screen()
{

}

void Screen::eventManagement()
{
    // GESTION DES EVENEMENTS
    SDL_Event event;
    while( SDL_PollEvent(&event))
    {
        switch(event.type)
        {
/*            case SDL_AUDIODEVICEADDED :
                event_AUDIODEVICEADDED(event);
            break;
            case SDL_AUDIODEVICEREMOVED :
                event_AUDIODEVICEREMOVED(event);
            break;
*/
            case SDL_CONTROLLERAXISMOTION :
                event_CONTROLLERAXISMOTION(event);
            break;
            case SDL_CONTROLLERBUTTONDOWN :
                event_CONTROLLERBUTTONDOWN(event);
            break;
            case SDL_CONTROLLERBUTTONUP :
                event_CONTROLLERBUTTONUP(event);
            break;
            case SDL_CONTROLLERDEVICEADDED :
                event_CONTROLLERDEVICEADDED(event);
            break;
            case SDL_CONTROLLERDEVICEREMOVED :
                event_CONTROLLERDEVICEREMOVED(event);
            break;
            case SDL_CONTROLLERDEVICEREMAPPED :
                event_CONTROLLERDEVICEREMAPPED(event);
            break;
            case SDL_DOLLARGESTURE :
                event_DOLLARGESTURE(event);
            break;
            case SDL_DOLLARRECORD :
                event_DOLLARRECORD(event);
            break;
            case SDL_DROPFILE :
                event_DROPFILE(event);
            break;
/*            case SDL_DROPTEXT :
                event_DROPTEXT(event);
            break;
            case SDL_DROPBEGIN :
                event_DROPBEGIN(event);
            break;
            case SDL_DROPCOMPLETE :
                event_DROPCOMPLETE(event);
            break;
*/
            case SDL_FINGERMOTION :
                event_FINGERMOTION(event);
            break;
            case SDL_FINGERDOWN :
                event_FINGERDOWN(event);
            break;
            case SDL_FINGERUP :
                event_FINGERUP(event);
            break;
            case SDL_KEYDOWN :
                event_KEYDOWN(event);
            break;
            case SDL_KEYUP :
                event_KEYUP(event);
            break;
            case SDL_JOYAXISMOTION :
                event_JOYAXISMOTION(event);
            break;
            case SDL_JOYBALLMOTION :
                event_JOYBALLMOTION(event);
            break;
            case SDL_JOYHATMOTION :
                event_JOYHATMOTION(event);
            break;
            case SDL_JOYBUTTONDOWN :
                event_JOYBUTTONDOWN(event);
            break;
            case SDL_JOYBUTTONUP :
                event_JOYBUTTONUP(event);
            break;
            case SDL_JOYDEVICEADDED :
                event_JOYDEVICEADDED(event);
            break;
            case SDL_JOYDEVICEREMOVED :
                event_JOYDEVICEREMOVED(event);
            break;
            case SDL_MOUSEMOTION :
                event_MOUSEMOTION(event);
            break;
            case SDL_MOUSEBUTTONDOWN :
                event_MOUSEBUTTONDOWN(event);
            break;
            case SDL_MOUSEBUTTONUP :
                event_MOUSEBUTTONUP(event);
            break;
            case SDL_MOUSEWHEEL :
                event_MOUSEWHEEL(event);
            break;
            case SDL_MULTIGESTURE :
                event_MULTIGESTURE(event);
            break;
            case SDL_QUIT :
                event_QUIT(event);
            break;
            case SDL_SYSWMEVENT :
                event_SYSWMEVENT(event);
            break;
            case SDL_TEXTEDITING :
                event_TEXTEDITING(event);
            break;
            case SDL_TEXTINPUT :
                event_TEXTINPUT(event);
            break;
            case SDL_USEREVENT :
                event_USEREVENT(event);
            break;
            case SDL_WINDOWEVENT :
                event_WINDOWEVENT(event);
            break;
            default:
            break;
        }
    }
}

void Screen::event_AUDIODEVICEADDED(SDL_Event &event){ }
void Screen::event_AUDIODEVICEREMOVED(SDL_Event &event){ }
void Screen::event_CONTROLLERAXISMOTION(SDL_Event &event){ }

void Screen::event_CONTROLLERBUTTONDOWN(SDL_Event &event)
{

}

void Screen::event_CONTROLLERBUTTONUP(SDL_Event &event)
{
}

void Screen::event_CONTROLLERDEVICEADDED(SDL_Event &event)
{
    int instanceID = app->engine->addController( event.cdevice.which );
    app->engine->associateControllerToPlayer(instanceID, false);
}

void Screen::event_CONTROLLERDEVICEREMOVED(SDL_Event &event)
{
    app->engine->removeController( event.cdevice.which );
}

void Screen::event_CONTROLLERDEVICEREMAPPED(SDL_Event &event){ }
void Screen::event_DOLLARGESTURE(SDL_Event &event){ }
void Screen::event_DOLLARRECORD(SDL_Event &event){ }
void Screen::event_DROPFILE(SDL_Event &event){ }
void Screen::event_DROPTEXT(SDL_Event &event){ }
void Screen::event_DROPBEGIN(SDL_Event &event){ }
void Screen::event_DROPCOMPLETE(SDL_Event &event){ }
void Screen::event_FINGERMOTION(SDL_Event &event){ }
void Screen::event_FINGERDOWN(SDL_Event &event){ }
void Screen::event_FINGERUP(SDL_Event &event){ }
void Screen::event_KEYDOWN(SDL_Event &event){ }
void Screen::event_KEYUP(SDL_Event &event){ }
void Screen::event_JOYAXISMOTION(SDL_Event &event){ }
void Screen::event_JOYBALLMOTION(SDL_Event &event){ }
void Screen::event_JOYHATMOTION(SDL_Event &event){ }
void Screen::event_JOYBUTTONDOWN(SDL_Event &event){ }
void Screen::event_JOYBUTTONUP(SDL_Event &event){ }
void Screen::event_JOYDEVICEADDED(SDL_Event &event){ }
void Screen::event_JOYDEVICEREMOVED(SDL_Event &event){ }
void Screen::event_MOUSEMOTION(SDL_Event &event){ }
void Screen::event_MOUSEBUTTONDOWN(SDL_Event &event){ }
void Screen::event_MOUSEBUTTONUP(SDL_Event &event){ }
void Screen::event_MOUSEWHEEL(SDL_Event &event){ }
void Screen::event_MULTIGESTURE(SDL_Event &event){ }

void Screen::event_QUIT(SDL_Event &event)
{
    app->engine->running = false;
}

void Screen::event_SYSWMEVENT(SDL_Event &event){ }
void Screen::event_TEXTEDITING(SDL_Event &event){ }
void Screen::event_TEXTINPUT(SDL_Event &event){ }
void Screen::event_USEREVENT(SDL_Event &event){ }

void Screen::event_WINDOWEVENT(SDL_Event &event)
{
    if( event.window.event == SDL_WINDOWEVENT_RESIZED )
    {
        glViewport( 0, 0, event.window.data1, event.window.data2 );
        glLoadIdentity();

        glOrtho(0, this->app->engine->display->width, this->app->engine->display->height, 0, -1, 1);
        this->app->engine->display->current_width = event.window.data1;
        this->app->engine->display->current_height = event.window.data2;
    }
}
