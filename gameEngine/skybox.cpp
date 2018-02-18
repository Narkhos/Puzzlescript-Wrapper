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
 
#include "skybox.hpp"

Skybox::Skybox(ThemeFileFinder* file_finder)
{
    skybox[SKY_LEFT]= new Texture();
    skybox[SKY_LEFT]->charger("skybox/left.png",file_finder);

    skybox[SKY_BACK]= new Texture();
    skybox[SKY_BACK]->charger("skybox/back.png",file_finder);

    skybox[SKY_RIGHT]= new Texture();
    skybox[SKY_RIGHT]->charger("skybox/right.png",file_finder);

    skybox[SKY_FRONT]= new Texture();
    skybox[SKY_FRONT]->charger("skybox/front.png",file_finder);

    skybox[SKY_UP]= new Texture();
    skybox[SKY_UP]->charger("skybox/up.png",file_finder);

    skybox[SKY_DOWN]= new Texture();
    skybox[SKY_DOWN]->charger("skybox/down.png",file_finder);
}

Skybox::~Skybox()
{
    for(int i = 0; i<6; ++i)
    {
        delete skybox[i];
        skybox[i] = NULL;
    }
}

void Skybox::afficher(float _size)
{
  bool b1=glIsEnabled(GL_TEXTURE_2D);     //new, we left the textures turned on, if it was turned on
        glDisable(GL_LIGHTING); //turn off lighting, when making the skybox
        glDisable(GL_DEPTH_TEST);       //turn off depth texting
        glEnable(GL_TEXTURE_2D);        //and turn on texturing
        skybox[SKY_BACK]->appliquer();  //use the texture we want
        glBegin(GL_QUADS);      //and draw a face
                //back face
                glTexCoord2f(0,1);
                glVertex3f(_size/2,-_size/2,_size/2);
                glTexCoord2f(1,1);
                glVertex3f(-_size/2,-_size/2,_size/2);
                glTexCoord2f(1,0);      //and repeat it...
                glVertex3f(-_size/2,_size/2,_size/2);
                glTexCoord2f(0,0);      //use the correct texture coordinate
                glVertex3f(_size/2,_size/2,_size/2);       //and a vertex
        glEnd();
        skybox[SKY_LEFT]->appliquer();
        glBegin(GL_QUADS);
                //left face
                glTexCoord2f(0,1);
                glVertex3f(-_size/2,-_size/2,_size/2);
                glTexCoord2f(1,1);
                glVertex3f(-_size/2,-_size/2,-_size/2);
                glTexCoord2f(1,0);
                glVertex3f(-_size/2,_size/2,-_size/2);
                glTexCoord2f(0,0);
                glVertex3f(-_size/2,_size/2,_size/2);
        glEnd();
        skybox[SKY_FRONT]->appliquer();
        glBegin(GL_QUADS);
                //front face
                glTexCoord2f(1,0);
                glVertex3f(_size/2,_size/2,-_size/2);
                glTexCoord2f(0,0);
                glVertex3f(-_size/2,_size/2,-_size/2);
                glTexCoord2f(0,1);
                glVertex3f(-_size/2,-_size/2,-_size/2);
                glTexCoord2f(1,1);
                glVertex3f(_size/2,-_size/2,-_size/2);
        glEnd();
        skybox[SKY_RIGHT]->appliquer();
        glBegin(GL_QUADS);
                //right face
                glTexCoord2f(0,1);
                glVertex3f(_size/2,-_size/2,-_size/2);
                glTexCoord2f(1,1);
                glVertex3f(_size/2,-_size/2,_size/2);
                glTexCoord2f(1,0);
                glVertex3f(_size/2,_size/2,_size/2);
                glTexCoord2f(0,0);
                glVertex3f(_size/2,_size/2,-_size/2);
        glEnd();
        skybox[SKY_UP]->appliquer();
        glBegin(GL_QUADS);
                //up face
                glTexCoord2f(1,0);
                glVertex3f(_size/2,_size/2,_size/2);
                glTexCoord2f(0,0);
                glVertex3f(-_size/2,_size/2,_size/2);
                glTexCoord2f(0,1);
                glVertex3f(-_size/2,_size/2,-_size/2);
                glTexCoord2f(1,1);
                glVertex3f(_size/2,_size/2,-_size/2);
        glEnd();
        skybox[SKY_DOWN]->appliquer();
        glBegin(GL_QUADS);
                //down face
                glTexCoord2f(1,0);
                glVertex3f(_size/2,-_size/2,-_size/2);
                glTexCoord2f(0,0);
                glVertex3f(-_size/2,-_size/2,-_size/2);
                glTexCoord2f(0,1);
                glVertex3f(-_size/2,-_size/2,_size/2);
                glTexCoord2f(1,1);
                glVertex3f(_size/2,-_size/2,_size/2);
        glEnd();
        glEnable(GL_LIGHTING);  //turn everything back, which we turned on, and turn everything off, which we have turned on.
        glEnable(GL_DEPTH_TEST);
        if(!b1)
                glDisable(GL_TEXTURE_2D);
}
