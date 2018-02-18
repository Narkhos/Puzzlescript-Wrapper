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
 
#include "engine.hpp"
#include "sprite2d.hpp"

Engine::Engine(string title, unsigned int native_width, unsigned int native_height, bool _network, bool activeControllerDetection)
    :cfg(ConfigSDL("config.xml")),
    fontPool(FontPool(&file_finder)),
    textManager(TextManager(cfg)), m_x(0),m_y(0),
    texturePool(&file_finder),
    animationPool(new AnimationPool(this,"data/sprites/", &file_finder)),
    currentTime(0),
    precTime(0)
{
    display = new Display(title, cfg, native_width, native_height);
    running = true;

    network = _network;
    // Initialise SDL_net (Note: We don't initialise or use normal SDL at all - only the SDL_net library!)
	if (network && SDLNet_Init() == -1)
	{
		cout << "Failed to intialise SDL_net: " << SDLNet_GetError() << endl;
		network = false;
	}

    this->activeControllerDetection = activeControllerDetection;
	foundControllers();
}

Engine::~Engine()
{
    if(network)
    {
        SDLNet_Quit();
    }

    delete animationPool;
    animationPool = NULL;

    SDL_Quit();
}

void Engine::update()
{
    precTime = currentTime;
    currentTime = SDL_GetTicks();
    SDL_GetMouseState(&m_x,&m_y);

    // on ramène la position de la souris aux dimension courantes de la fenêtre
    // pour le cas ou la taille de la fenêtre est différente des dimensions natives de l'application
    m_x = m_x * display->width / display->current_width;
    m_y = m_y * display->height / display->current_height;
}

int Engine::addController( int id )
{
    if( SDL_IsGameController( id ) ) {
        SDL_GameController *pad = SDL_GameControllerOpen( id );
        cout << "Controller Id : " << id << endl;
        if( pad )
        {
            SDL_Joystick *joy = SDL_GameControllerGetJoystick( pad );
            int instanceID = SDL_JoystickInstanceID( joy );

			controllers[instanceID] = pad;
            cout << "Branchement du controller " << instanceID << endl;
            return instanceID;

        }
    }
    return -1;
}

int Engine::removeController( int id )
{
    cout << "Debranchement du controller " << id << endl;
    SDL_GameController *pad = controllers[id];
    SDL_GameControllerClose( pad );
	controllers[id] = NULL;
	controllers.erase(id);

	for(int i = 0; i<this->playerControllers.size();i++)
	{
        // On retire l'association player/controller si elle existe
        if(this->playerControllers[i]==id)
        {
            this->playerControllers[i] = -1;
            return i; // Le joueur "i" avait ce controller d'attribué
        }
	}
	return -1; // aucun joueur n'avait ce controller d'attribué
}

void Engine::foundControllers()
{
    if(this->activeControllerDetection)
    {
        for (int i = 0; i < SDL_NumJoysticks(); ++i)
        {
            addController(i);
        }
    }
}

int Engine::associateControllerToPlayer(int playerId, int controllerId)
{
    if(controllers[controllerId]!=NULL)
    {
        if(playerId>=0 && playerId <playerControllers.size())
        {
            playerControllers[playerId] = controllerId;
            return playerId;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int Engine::getPlayerFromController(int controllerId)
{
    if(controllerId<0) return -1;

    for(int i = 0; i < playerControllers.size(); i++)
    {
        if(playerControllers[i] == controllerId)
        {
            return i;
        }
    }

    return -1;
}

int Engine::associateControllerToPlayer(int controllerId, bool add)
{
    if(controllers[controllerId]!=NULL && getPlayerFromController(controllerId)==-1)
    {
        // Si un joueur existant n'avait pas ou plus de controller, on lui affecte celui-ci
        for(int i = 0; i < playerControllers.size(); i++)
        {
            if(playerControllers[i] == -1)
            {
                playerControllers[i] = controllerId;
                return i;
            }
        }
        // Si aucun joueur sans controller n'a été trouvé, on crée un nouveau joueur et on lui affecte le controller
        if(add)
        {
            playerControllers.push_back(controllerId);
            return playerControllers.size()-1; // renvoi le rang du nouveau joueur
        }
    }
    else
    {
        return -1;
    }
}

int Engine::countPlayersWithController()
{
    int n = 0;
    for(int i = 0; i < playerControllers.size(); i++)
    {
        if(playerControllers[i] != -1)
        {
            n++;
        }
    }

    return n;
}

void Engine::clearControllerToPlayer()
{
    playerControllers.clear();
}

int Engine::getDeltaTime()
{
    return this->currentTime-this->precTime;
}
