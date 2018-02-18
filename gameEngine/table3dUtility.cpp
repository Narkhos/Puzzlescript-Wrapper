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
 
#include "table3dUtility.hpp"

// class Pos3d

Pos3d::Pos3d(int _x, int _y, int _z):x(_x),y(_y),z(_z)
{

}

Pos3d::Pos3d(const Pos3d& p)
{
    //cout << "Pos3d : constructeur par copie " << endl;
    x=p.x;
    y=p.y;
    z=p.z;
}

Pos3d& Pos3d::operator+=(const Pos3d& p)
{
    this->x+=p.x;
    this->y+=p.y;
    this->z+=p.z;
    return *this; // return the result by reference
}

Pos3d& Pos3d::operator-=(const Pos3d& p)
{
    this->x-=p.x;
    this->y-=p.y;
    this->z-=p.z;
    return *this; // return the result by reference
}

Pos3d  Pos3d::operator + (const Pos3d& p) const
{
    return Pos3d(x+p.x,y+p.y,z+p.z);
}

Pos3d  Pos3d::operator - (const Pos3d& p) const
{
    return Pos3d(x-p.x,y-p.y,z-p.z);
}

bool Pos3d::operator ==(const Pos3d& p) const
{
    if(this->x==p.x && this->y==p.y && this->z==p.z) return true;

    return false;
}

string Pos3d::getHashCode() const
{
    stringstream convert;
    convert << setw(2) << x << setw(2) << y << setw(2) << z;
    return convert.str();
}

// Manhattan Distance
int Pos3d::manhattanDistance(const Pos3d& p1, const Pos3d& p2)
{
    return abs(p1.x-p2.x)+abs(p1.y-p2.y)+abs(p1.z-p2.z);
}


std::ostream& operator<<(std::ostream& os, const Pos3d& p)
{
    os <<"["<< p.x << ","<<p.y<<","<<p.z<<"]";
    return os;
}

enum Direction* trouveDirectionVoisin(const Pos3d &orig, const Pos3d &dest)
{
	Vec3d VecDirection = dest-orig;
	for(int i = 0; i < 26; ++i)
	{
		if(Voisins[i]==VecDirection)
		{

			return new enum Direction((enum Direction)i);
		}
	}

	return NULL;
}
