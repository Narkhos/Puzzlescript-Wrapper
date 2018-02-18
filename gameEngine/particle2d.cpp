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
 
#include "particle2d.hpp"

//class Particle2d
Particle2d::Particle2d(int _tex, float _x0, float _y0, int _t0, int _d, float _v0, float _a0, float _width)
    : x(_x0), y(_y0), x0(_x0), y0(_y0), tex(_tex), t0(_t0), d(_d), v0(_v0), a0(_a0), width(_width)
{
    alpha = 1.0;
}

Particle2d::~Particle2d()
{
}

bool Particle2d::update(int t, float g, float wind)
{
    float dt = t-t0;
    if(dt>d) return false;

    x=dt*(v0*cos(a0)+wind)+x0;
    y=(dt*(v0*sin(a0)+dt*(1.0/2.0*g)))+y0;

    alpha = 1.0-(float)dt/(float)d;
    return true;
}

void Particle2d::draw(float scroll_x, float scroll_y)
{
    // TODO, gérer la texture et les dimentions de la particule de façon paramétrée
    // On pourrait aussi gérer la luminosité de cette manière
    drawImage(tex,x+scroll_x,y+scroll_y,width,width,alpha, 1.0, NULL);
}

// class Particle2dGenerator


Particle2dGenerator::Particle2dGenerator(Engine* _engine, string img_particle, float _g, float _x, float _y, float _v0, float _a0_min, float _a0_max, float _flow, int _nb_max, int _d, float _width)
    : engine(_engine), g(_g), v0(_v0), flow(_flow), nb_max(_nb_max), d(_d), x(_x), y(_y), a0_min(_a0_min), a0_max(_a0_max), width(_width)
{
    if(a0_min > a0_max)
    {
        float tmp = a0_max;
        a0_max = a0_min;
        a0_min = tmp;
    }

    texture = engine->texturePool.getTexture(img_particle);
    //image = new Image(img_particle);
    prec_t=0;
}

Particle2dGenerator::~Particle2dGenerator()
{
    //delete image;
    //image = NULL;
}

void Particle2dGenerator::update(int t, bool fixe, float wind)
{
    for(int i = particles.size()-1; i >= 0; --i)
    {
        if(fixe)
        {
            particles[i].x0=this->x;
            particles[i].y0=this->y;
        }

        if(!particles[i].update(t, g, wind))
        {
            particles.erase(particles.begin()+i);
        }
    }

    if(t-prec_t >= flow)
    {
        prec_t = t;

        if(particles.size() < nb_max)
        {
            //particles.push_back(Particle2d(image->tex, x, y, t, getDureeDeVie(), getVitesseInitiale(), getAngleInitial(),getWidth()));
            particles.push_back(Particle2d(texture->getId(), x, y, t, getDureeDeVie(), getVitesseInitiale(), getAngleInitial(),getWidth()));
        }
    }
}

// Surchargeable pour faire varier aléatoirement les paramètre pour chaque particule
float Particle2dGenerator::getVitesseInitiale()
{
    return v0;
}

float Particle2dGenerator::getAngleInitial()
{
    float amplitude = a0_max - a0_min;
    return a0_min + amplitude*(float)(rand()%100)/100.0;

    //return (a0_min+a0_max)/2.0;
}

int Particle2dGenerator::getDureeDeVie()
{
    return d+rand()%(d);
    //return d;
}

float Particle2dGenerator::getWidth()
{
    return width;
}

void Particle2dGenerator::draw(float scroll_x, float scroll_y)
{
    for(int i = particles.size()-1; i >= 0; --i)
    {
        particles[i].draw(scroll_x, scroll_y);
    }
}
