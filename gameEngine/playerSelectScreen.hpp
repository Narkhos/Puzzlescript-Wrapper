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
 
#ifndef _PLAYER_SELECT_SCREEN_HPP_
#define _PLAYER_SELECT_SCREEN_HPP_

#include "application.hpp"

class ScreenPlayerSelect : public Screen
{
	public:

    SDL_Color textColor;
	GUI_TexteDynamique *txtGame;
	int minPlayers;
    vector<Texture*> playerBg;
    Texture* pressStart;
    Texture* ready;
    GUI_Button* go;

	ScreenPlayerSelect(Application* app, int minPlayers, int _maxPlayers);
	~ScreenPlayerSelect();

	void update();

	void draw();
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);

	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_CONTROLLERBUTTONDOWN(SDL_Event &event);
};

#endif
