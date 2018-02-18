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
 
#ifndef __RAND_GEN_HPP__
#define __RAND_GEN_HPP__
#include <cstdlib>

using namespace std;

class RandomGenerator
{
	public:
	int m_initValue; // valeur utilisée pour initialiser le générateur

	RandomGenerator(int initValue)
	{
		init(initValue);
	}

	~RandomGenerator()
	{}

	void init(int initValue)
	{
		m_initValue = initValue;
		srand(initValue);
	}

	float fGetRand(float a, float b)
	{
		return ( rand()/(float)RAND_MAX ) * (b-a) + a;
	}

	int sGetRand(int a, int b)
	{
		return rand()%(b-a) + a;
	}

};

#endif
