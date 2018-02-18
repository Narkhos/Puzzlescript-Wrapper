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
 
#include "timer.hpp"
#include <iostream>
using namespace std;

Timer::Timer()
    :m_fTime(0.0f), m_fSpeed(1.0f), m_fDelta(0.0f)
        , m_bStopped(true), m_iLastTickTime(0)
{}

Timer::~Timer()
{}

float Timer::getSpeed () const
{
    return m_fSpeed;
}

unsigned int Timer::getTime () const
{
    return (unsigned int)m_fTime;
}

float Timer::getDelta() const
{
    return m_fDelta;
}

bool Timer::isStopped () const
{
    return m_bStopped;
}

void Timer::setSpeed (float speed)
{
    m_fSpeed = speed;
}

void Timer::setTime (unsigned int time)
{
    m_fTime = (float)time;
    m_fDelta = 0.0f;
}

void Timer::start ()
{
    // Si le timer était bien en stop, on initialise l'instant précédent
    // à l'instant courrant
    if(m_bStopped)
    {
        m_iLastTickTime = SDL_GetTicks();
        m_bStopped = false;
//        cout << "In Start" << endl;
    }
}

void Timer::stop ()
{
    if(!m_bStopped)
    {
        // on fait un dernier tick avant de mettre le timer en pause
        // cela permet de repartir du bon pied au start
        tick();
        m_bStopped = true;
//        cout << "In Stop" << endl;
    }

}

void Timer::tick ()
{
    // Si le timer est en route, on le met à jour
    if(!m_bStopped)
    {
        unsigned int iCurrentDeviceTime = SDL_GetTicks();

        m_fDelta = m_fSpeed * ( (float)iCurrentDeviceTime - (float)m_iLastTickTime);
		//cout << "Delta : " << m_fDelta << endl;
        m_fTime += m_fDelta;
        m_iLastTickTime = iCurrentDeviceTime;
    }
}
