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
 
#include "playerSelectScreen.hpp"

// class ScreenPlayerSelect : public Screen

ScreenPlayerSelect::ScreenPlayerSelect(Application* app, int _minPlayers, int _maxPlayers)
    :Screen(app,"PlayerSelect"), minPlayers(_minPlayers)
{
    textColor = {255 , 255 , 255, 255};
	txtGame = new GUI_TexteDynamique(L"Player Selection", app->engine->fontPool.getFont("OxygenMono-Regular.ttf"), textColor);

	// Chargement des images de fond correspondant aux différents joueurs
	for(int i = 0; i<_maxPlayers; i++)
    {
        stringstream ss;
        ss << "playerSelect/player" << i+1 << ".png";
        playerBg.push_back(this->app->engine->texturePool.getTexture(ss.str()));
    }

    pressStart = this->app->engine->texturePool.getTexture("playerSelect/pressStart.png");
    ready = this->app->engine->texturePool.getTexture("playerSelect/ready.png");
    go = new GUI_Button(this->app->engine->texturePool.getTexture("playerSelect/go.png")->getId(),
        app->engine->display->width/2 - 50, app->engine->display->height-80, 100,40, 2, NULL, L"Start Game");
}
ScreenPlayerSelect::~ScreenPlayerSelect() {}

void ScreenPlayerSelect::update()
{
    // Bouton Go
    if(app->engine->countPlayersWithController()>= this->minPlayers)
    {
        go->etat=1;
        go->actif=true;
    }
    else
    {
        go->etat=0;
        go->actif=false;
    }
}

void ScreenPlayerSelect::event_KEYDOWN(SDL_Event& event)
{
    app->setScreen("MainMenu");
}

void ScreenPlayerSelect::event_MOUSEBUTTONDOWN(SDL_Event& event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        if(go->actif && go->clique(app->engine->m_x, app->engine->m_y))
        {
            app->setScreen("Game");
        }
    }
}

void ScreenPlayerSelect::event_CONTROLLERBUTTONDOWN(SDL_Event &event)
{
    if(event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
    {
        this->app->engine->associateControllerToPlayer(event.cbutton.which, true);

        for(int i = 0; i < this->app->engine->playerControllers.size(); i++)
        {
            cout << "Player " << i << " : " << this->app->engine->playerControllers[i] << endl << endl;
        }
    }

    // Gestion des
}

void ScreenPlayerSelect::onSet(string id_orig)
{
    app->engine->clearControllerToPlayer();
}

void ScreenPlayerSelect::onQuit(string id_dest)
{
    if(id_dest=="Game")
    {
        cout << "C'est parti !" << endl;
    }
    else if(id_dest=="MainMenu")
    {
        cout << "Ah, bon ... " << endl;
    }
}

void ScreenPlayerSelect::draw()
{
    // Afficher le jeu
    txtGame->draw(10,50);

    for(int i = 0; i < playerBg.size();i++)
    {
        drawImage(playerBg[i]->getId(),i*(app->engine->display->width/playerBg.size()),0,app->engine->display->width/playerBg.size(), 4.0/5.0*app->engine->display->height,1.0,1.0);

        if(i >= app->engine->playerControllers.size() || app->engine->playerControllers[i]==-1)
        {
            drawImage(pressStart->getId(),
                (1.0/12.0+i)*(app->engine->display->width/playerBg.size()),
                6.0/10.0 * app->engine->display->height,
                10.0/12.0*app->engine->display->width/playerBg.size(),
                1.0/12.0*app->engine->display->height,
                1.0/2.0*sin((float)(app->engine->currentTime)/200.0)+1.0/2.0,
                1.0);
        }
        else
        {
            // Le joueur est pret (TODO voir comment on gère les éventuels paramètres, notamment le choix du personnage s'il y a lieu)
            drawImage(ready->getId(),
                (1.0/12.0+i)*(app->engine->display->width/playerBg.size()),
                6.0/10.0 * app->engine->display->height,
                10.0/12.0*app->engine->display->width/playerBg.size(),
                1.0/12.0*app->engine->display->height,
                1.0,
                1.0);
        }
    }

    go->draw();
}
