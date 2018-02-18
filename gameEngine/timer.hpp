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
 
#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <SDL2/SDL.h>

class Timer
{
    private:
        float m_fSpeed;
        float m_fTime;
        float m_fDelta;
        bool m_bStopped;
        unsigned int m_iLastTickTime;
    public:
    Timer();
    ~Timer();
    unsigned int	getRealTime () const;
    float getSpeed () const;
    unsigned int getTime () const;
    float getDelta() const;
    bool isStopped () const;
    void setSpeed (float speed=1.0f);
    void setTime (unsigned int time);
    void start ();
    void stop ();
    void tick ();
	unsigned int getCurrentDeviceTime();

};

#endif
