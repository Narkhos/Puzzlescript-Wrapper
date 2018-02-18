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
 
#include "puyoRand.hpp"

// class RandGen

typeRand RandGen::getType() { return this->type; }

RandGen::RandGen(unsigned int _seed, typeRand _type):seed(_seed), type(_type)
{
}

RandGen::~RandGen() {}
 
// class RandGenTsu

RandGenTsu::RandGenTsu(unsigned int _seed):RandGen(_seed, TSU)
{
	d0 = 0x4AD00000;
	sp = (unsigned int) 0x00FF7FF4; /*  ou 0x00FF7FE8 */
	
	if(sizeof(unsigned int) != 4)
	{
		cout << "Pour fonctionner correctement, le generateur doit fonctionner sur des entiers non signés sur 4 octets." << endl;
		cout << "La taille des entiers non signes sur ce systeme est : " << sizeof(unsigned int) << endl;
		exit(1);
	}
}

RandGenTsu::~RandGenTsu() {}
	

unsigned int RandGenTsu::nextVal()
{
	/* 
		d0 value depends from where the function is called, 
		so is a yet unknown variable.
		The following initialization is accurate for the first 7 values of the RNG, 
		if you let the animation play through, on the genesis.

		For the color-set shuffling, d0 holds 0x00000004.
		For the pool randomization process, d0 holds the previous RNG value masked 
		with 0xFFFF00FF, and is initialized to 0x4AD00000.
	*/

	d0 += seed;
	d0 += sp;
	d0  = this->rotl(d0,5);
	d0 += seed;
	d0++;
	seed = d0;
	d0 = seed & 0xFFFF00FF;
	return seed;
}

unsigned int RandGenTsu::min()
{
	return 0;
}

unsigned int RandGenTsu::max()
{
	return UINT_MAX;
}


unsigned int RandGenTsu::rotl(const unsigned int value, int shift)
{
	if ((shift &= sizeof(value)*8 - 1) == 0) return value;
	return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

unsigned int RandGenTsu::rotr(const unsigned int value, int shift)
{
	if ((shift &= sizeof(value)*8 - 1) == 0) return value;
	
	return (value >> shift) | (value << (sizeof(value)*8 - shift));
}

// class RandGenVS

RandGenVS::RandGenVS(unsigned int _seed):RandGen(_seed, VS)
{
	mt.seed(seed);
}

unsigned int RandGenVS::nextVal()
{
	return mt();
}

unsigned int RandGenVS::min()
{
	return mt.min();
}

unsigned int RandGenVS::max()
{
	return mt.max();
}