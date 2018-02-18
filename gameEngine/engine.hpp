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
 
#ifndef __GAME_ENGINE_HPP__
#define __GAME_ENGINE_HPP__

#include <iostream>
#include "chemins.h"
#include "configSDL.hpp"
#include "display.hpp"
#include "theme.h"
#include "gui.hpp"
#include "animation.hpp"

#include <SDL2/SDL_net.h>

using namespace std;


class AnimationPool;

class Engine
{
    public:

    ConfigSDL cfg;
    Display *display;
    ThemeFileFinder file_finder;
    FontPool fontPool;
    TexturePool texturePool;
    TextManager textManager;
    AnimationPool *animationPool;
    map<int, SDL_GameController *> controllers; ///< Liste des controllers branchés
    bool activeControllerDetection;
    vector<int> playerControllers;  ///< Liste des id controller associés aux joueurs

    bool running;
    bool network;

    int m_x;							///< position x de la souris
	int m_y;							///< position y de la souris
    int currentTime;                    ///< temps courant obtenu avec SDL_GetTicks()
    int precTime;                       ///< temps précédent obtenu avec SDL_GetTicks()
    Engine(string title, unsigned int native_width=0, unsigned int native_height=0, bool _network=true, bool activeControllerDetection=true);
    ~Engine();
    void update();

    int addController( int id );
    /**
        retire un controller qui a été débranché
        @param id identifiant du controller à retirer
        @return id du joueur auquel le controller était associé ou -1 si pas de joueur associé
    */
    int removeController( int id );
    void foundControllers();

    /**
        Associe un controller à un joueur
        @param playerId rang du joueur
        @param controllerId Id du controller à lui associer
        @return Id du player ou -1 si l'association n'a pas pu se faire
    */
    int associateControllerToPlayer(int playerId, int controllerId);
    /**
        Associe un controller au prochain joueur qui n'en a pas encore
        @param controllerId Id du controller à lui associer
        @return Id du player ou -1 si l'association n'a pas pu se faire
    */
    int associateControllerToPlayer(int controllerId, bool add);

    int getPlayerFromController(int controllerId);

    int countPlayersWithController();
    void clearControllerToPlayer();
    int getDeltaTime();

};

#endif
