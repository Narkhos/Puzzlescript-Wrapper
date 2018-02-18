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
 
#ifndef __TABLE3DUTILITY_HPP__
#define __TABLE3DUTILITY_HPP__
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
using namespace std;

class Pos3d
{
	public:
	int x;
	int y;
	int z;

	Pos3d(int _x=0, int _y=0, int _z=0);

	Pos3d(const Pos3d& p);

	Pos3d& operator+=(const Pos3d& p);

	Pos3d& operator-=(const Pos3d& p);

	Pos3d  operator + (const Pos3d& p) const;

	Pos3d  operator - (const Pos3d& p) const;

	bool operator ==(const Pos3d& p) const;

	string getHashCode() const;

	// Mahnhattan Distance
	static int manhattanDistance(const Pos3d& p1, const Pos3d& p2);
};

std::ostream& operator<<(std::ostream& os, const Pos3d& p);

typedef Pos3d Vec3d;

enum Direction
{
    UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3,
    _IN = 4,
    _OUT = 5, // TODO : les mots IN et OUT semblent poser problème à la compilation. Revoir le nommage des Directions
	DOWNRIGHT = 6, UPRIGHT = 7, UPLEFT = 8, DOWNLEFT = 9,
	INDOWN =10, OUTDOWN=11, OUTUP=12, INUP=13,
	INRIGHT=14, OUTRIGHT=15, OUTLEFT=16, INLEFT=17,
	DOWNRIGHTOUT=18, DOWNLEFTOUT=19, UPRIGHTOUT=20, UPLEFTOUT=21,
	DOWNRIGHTIN=22, DOWNLEFTIN=23, UPRIGHTIN=24, UPLEFTIN=25
};

const Direction Opposee[26] = {DOWN, UP, RIGHT, LEFT,_OUT, _IN, UPLEFT, DOWNLEFT, DOWNRIGHT, UPRIGHT,
				OUTUP, INUP, INDOWN, OUTDOWN,
				OUTLEFT, INLEFT, INRIGHT, OUTRIGHT,
				UPLEFTIN, UPRIGHTIN, DOWNLEFTIN, DOWNRIGHTIN,
				UPLEFTOUT, UPRIGHTOUT, DOWNLEFTOUT, DOWNRIGHTOUT
};



const Vec3d Voisins[26]={Vec3d(0,-1,0),Vec3d(0,1,0),Vec3d(-1,0,0),Vec3d(1,0,0),Vec3d(0,0,1),Vec3d(0,0,-1),
					Vec3d(1,1,0),Vec3d(1,-1,0),Vec3d(-1,-1,0),Vec3d(-1,1,0),
					Vec3d(0,1,1),Vec3d(0,-1,1),Vec3d(0,-1,-1),Vec3d(0,-1,1),
					Vec3d(1,0,1),Vec3d(1,0,-1),Vec3d(-1,0,-1),Vec3d(-1,0,1),
					Vec3d(1,1,-1),Vec3d(-1,1,-1),Vec3d(1,-1,-1),Vec3d(-1,-1,-1),
					Vec3d(1,1,1),Vec3d(-1,1,1),Vec3d(1,-1,1),Vec3d(-1,-1,1)};

enum Direction* trouveDirectionVoisin(const Pos3d &orig, const Pos3d &dest);

#endif
