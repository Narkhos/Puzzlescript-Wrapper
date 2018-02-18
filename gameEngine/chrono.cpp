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
 
#include "chrono.hpp"

Chrono::Chrono(TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor, Timer *_pTimer, int _iStartingTime)
	: GUI_TexteDynamique(L"",_font,_couleur,_bgColor), m_pTimer(_pTimer),m_iStartingTime(_iStartingTime)
{

}

Chrono::~Chrono()
{
}

void Chrono::setStartingTime(int _iStartingTime)
{
		m_iStartingTime=_iStartingTime;
}

void Chrono::vDraw(float _x, float _y, wstring prefixe)
{
    int time = m_pTimer->getTime() - m_iStartingTime;
	wstring timeString = L"";

    wstringstream ss;
    ss << prefixe;
    ss << Chrono::getTime_wstring(time,false);

    timeString = ss.str();
    this->setTexte(timeString);

	this->draw(_x, _y);

}

wstring Chrono::getTime_wstring(int time, bool millisecondes)
{
    int heures, minutes, secondes;
	wstring timeString = L"";

	//time = ((Timer*)m_pTimer)->getCurrentDeviceTime();
	heures = time / 3600000;
	if (heures > 99) heures = 99;
	if (heures < 10)
	time = time % 3600000;
	minutes = time / 60000;
	time = time % 60000;
	secondes = time / 1000;
	time = time % 1000;

    wstringstream ss;

    ss << ((heures < 10)?L"0":L"");
    ss << heures << L":";
    ss << ((minutes < 10)?L"0":L"");
    ss << minutes << L":";
    ss << ((secondes < 10)?L"0":L"");
    ss << secondes;

    if(millisecondes)
    {
        // TODO : lorsqu'on utilise les millisecondes cela provoque un plantage au bout d'un moment, sans doute lié à une fuite de mémoire au niveau de la génération de la texture de Texte
        ss << L":";
        ss << ((time < 10)?L"0":L"");
        ss << ((time < 100)?L"0":L"");
        ss << time;
    }

    return ss.str();
}
