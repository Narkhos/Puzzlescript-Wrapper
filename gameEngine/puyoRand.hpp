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

#ifndef __PUYORAND_HPP__
#define __PUYORAND_HPP__

#include <math.h>		// std::trunc
#include <stdlib.h>
#include <climits>		// UINT_MAX
#include <iostream>		// std::cout
#include <random>		// std::mt19937

using namespace std;

enum typeRand {TSU, VS, NAZO};

/**
	Générateur de nombre aléatoire
*/
class RandGen
{
	protected:
	
	unsigned int seed;	///< graine de génération des nombres
	typeRand type;
	
	public:
	
	typeRand getType();
	
	/**
		@param _seed graine de génération des nombres
	*/
	RandGen(unsigned int _seed, typeRand _type);
	~RandGen();
	
	/**
		Retourne la valeur aléatoire suivante
		@return valeur aléatoire générée
	*/
	virtual unsigned int nextVal()=0;
	
	/**
		Valeur minimum du générateur aléatoire
		@return Valeur minimum du générateur aléatoire
	*/
	virtual unsigned int min()=0;
	
	/**
		Valeur maximum du générateur aléatoire
		@return Valeur maximum du générateur aléatoire
	*/
	virtual unsigned int max()=0;
};

/**
	Générateur de nombre aléatoire basé sur l'algorithme utilisé dans Puyo Puyo Tsu
*/
class RandGenTsu : public RandGen
{
	private:
	
	unsigned int sp;
	unsigned int d0;
	
	public:
	
	/**
		@param _seed graine de génération des nombres
	*/
	RandGenTsu(unsigned int _seed);
	~RandGenTsu();
	
	/**
		Retourne la valeur aléatoire suivante
		@return valeur aléatoire générée
	*/
	unsigned int nextVal();
	
	/**
		Valeur minimum du générateur aléatoire
		@return Valeur minimum du générateur aléatoire
	*/
	virtual unsigned int min();
	
	/**
		Valeur maximum du générateur aléatoire
		@return Valeur maximum du générateur aléatoire
	*/
	virtual unsigned int max();

	protected:
	
	/**
		décalage binaire vers la gauche
		@param value valeur à traiter
		@param shift nombre de bits de décalage
	*/
	unsigned int rotl(const unsigned int value, int shift);
	
	/**
		décalage binaire vers la droite
		@param value valeur à traiter
		@param shift nombre de bits de décalage
	*/
	unsigned int rotr(const unsigned int value, int shift);
};

class RandGenVS:public RandGen
{
	private:
	
	mt19937 mt;		///< Mersenne Twister 19937 generator
		
	public:
	
	RandGenVS(unsigned int _seed);
	
	/**
		Retourne la valeur aléatoire suivante
		@return valeur aléatoire générée
	*/
	unsigned int nextVal();
	
		/**
		Valeur minimum du générateur aléatoire
		@return Valeur minimum du générateur aléatoire
	*/
	virtual unsigned int min();
	
	/**
		Valeur maximum du générateur aléatoire
		@return Valeur maximum du générateur aléatoire
	*/
	virtual unsigned int max();
};

#endif