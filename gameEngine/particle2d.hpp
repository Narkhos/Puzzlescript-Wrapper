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
 
#ifndef __PARTICLE2D_HPP__
#define __PARTICLE2D_HPP__

#include <iostream>
#include "engine.hpp"

using namespace std;

class Particle2d
{
	public:
	float x;
	float y;
    float x0;
	float y0;
	int tex;
	int t0;
	int d;
	float a0;
	float v0;
	float width;
	float alpha;

	Particle2d(int _tex, float _x0, float _y0, int _t0, int _d, float _v0, float _a0, float _width);

	~Particle2d();

	bool update(int t, float g, float wind);

	void draw(float scroll_x, float scroll_y);

};

class Particle2dGenerator
{
	public:

	float g;
	float v0;
	float a0_min;
	float a0_max;
	float flow;
	int tex;
	int nb_max;
	int prec_t;
	float x;
	float y;
	int d;
    float width;
    Engine* engine;
    Texture* texture;
	//Image * image;

	vector<Particle2d> particles;

	Particle2dGenerator(Engine* _engine, string img_particle, float _g, float _x, float _y, float _v0, float _a0_min, float _a0_max, float _flow, int _nb_max, int _d, float _width);

	~Particle2dGenerator();

	void update(int t, bool fixe,float wind);

	// Surchargeable pour faire varier aléatoirement les paramètre pour chaque particule
	virtual float getVitesseInitiale();

	virtual float getAngleInitial();

	virtual int getDureeDeVie();

	virtual float getWidth();

	void draw(float scroll_x, float scroll_y);

};

#endif
