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
 
#ifndef __CHRONO_HPP__
#define __CHRONO_HPP__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "timer.hpp"
#include "gui.hpp"

using namespace std;

class Chrono : public GUI_TexteDynamique
{
	protected:
	Timer *m_pTimer;
	int m_iStartingTime;

	public:
	Chrono(TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor, Timer *_pTimer, int _iStartingTime);
	~Chrono();
	void setStartingTime(int _uStartingTime);
	void vDraw(float _x, float _y, wstring prefixe = L"");
	static wstring getTime_wstring(int time, bool millisecondes);

};


#endif
