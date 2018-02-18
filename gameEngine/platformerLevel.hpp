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
 
#ifndef __PLATFORMER_LEVEL_HPP__
#define __PLATFORMER_LEVEL_HPP__

#include "table3d.hpp"
#include <sstream>

using namespace std;

class PlatformerCell
{
    public:
    Engine* engine;
    Texture* tex;

    PlatformerCell(string cheminTexture = "caseVide.png", Engine* _engine=NULL)
        :engine(_engine)
    {
        if(engine != NULL)
        {
            tex = engine->texturePool.getTexture(cheminTexture);
        }
        /*SDL_Surface* img =IMG_Load("themes/default/textures/caseInvPleine.png");
        tex_pleine = SurfaceToTexture2(img);
        SDL_FreeSurface(img); img = NULL;*/
    }

    ~PlatformerCell()
    {
    }
    void trace()
    {
    }
    void copyToClipBoard(stringstream &ss)
    {
    }
    void draw(float x, float y, float w, float h)
    {
        drawImage(tex->getId(),x,y,w,h,1.0);
    }
};

class PlatformerLevel : public Table3d<PlatformerCell>
{
    public:

    Engine* engine;

    PlatformerLevel(Engine* _engine)
    {
    }

    PlatformerLevel(Engine* _engine, int _width, int _height)
        :Table3d(_width, _height, 1, false), engine(_engine)
    {
        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                data[x][y][0] = new PlatformerCell("caseVide.png",this->engine);

            }
        }
    }

    ~PlatformerLevel()
    {
    }

    bool init(string _data, int w, int h)
    {
        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                switch(_data[x+y*(w+1)])
                {
                    case 'x':
                        data[x][y][0] = new PlatformerCell("casePleine.png", this->engine);
                    break;
                    default:
                        data[x][y][0] = new PlatformerCell("caseVide.png", this->engine);
                }


            }
        }
    }

    void draw(int pos_x, int pos_y)
    {
        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                if(data[x][y][0] != NULL)
                {
                    data[x][y][0]->draw(pos_x+x*32, pos_y+y*32, 32, 32);
                }
            }
        }
    }
};

#endif
