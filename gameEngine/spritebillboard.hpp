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
 
#include "sprite2d.hpp"

class SpriteBillboard : public Sprite2d
{
	public:

	float ambient[4];
	float diffuse[4];
	float specular[4];

    bool left;
	glm::vec2 size;

	SpriteBillboard(Timer *_pTimer, glm::vec2 _size, ThemeFileFinder* _file_finder):Sprite2d(NULL,_pTimer,_file_finder),size(_size)
	{
	    for(int i = 0; i < 4; ++i)
        {
            ambient[i]=1.0;
            diffuse[i]=1.0;
            specular[i]=0.0;
        }
	}

	~SpriteBillboard()
	{
	}

    void setDirection(bool _left)
    {
        this->left = _left;
    }

	void draw(glm::mat4 M)
	{
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.4);

		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glm::vec3 CameraRight_worldspace = {M[0][0], M[1][0], M[2][0]};
		glm::vec3 CameraUp_worldspace = {M[0][1], M[1][1], M[2][1]};
		glm::vec3 squareVertices[4] = {glm::vec3(-0.5f, -0.5f, 0.0f),glm::vec3(0.5f, -0.5f, 0.0f),glm::vec3(0.5f, 0.6f, 0.0f),glm::vec3(-0.5f, 0.6f, 0.0f)};


		for(int i = 0; i < 4; ++i)
		{
			squareVertices[i] =
				glm::vec3(0.0,0.0,0.5)
				+ CameraRight_worldspace * squareVertices[i].x * this->size.x
				+ CameraUp_worldspace * squareVertices[i].y * this->size.y;
		}

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, 0.0);

		float t[8];


		this->appliquer(t);

        if(this->left)
        {
            float tmp;
            tmp = t[0];
            t[0] = t[2];
            t[2] = tmp;
            tmp = t[4];
            t[4] = t[6];
            t[6] = tmp;
        }

		glNormal3f(0.0, 1.0, 0.0);
		glBegin(GL_QUADS);

		//glTexCoord2f (0.0, 1.0);
		glTexCoord2f (t[0], t[1]);
		//glTexCoord2f (0.25*(date/300%4), 1.0);
		glVertex3f(squareVertices[0].x, squareVertices[0].y, squareVertices[0].z);

		//glTexCoord2f (0.25, 1.0);
		glTexCoord2f (t[2], t[3]);
		//glTexCoord2f (0.25*(date/300%4)+0.25, 1.0);
		glVertex3f(squareVertices[1].x, squareVertices[1].y, squareVertices[1].z);

		//glTexCoord2f (0.25, 0.0);
		glTexCoord2f (t[4], t[5]);
		//glTexCoord2f (0.25*(date/300%4)+0.25, 0.0);
		glVertex3f(squareVertices[2].x, squareVertices[2].y, squareVertices[2].z);

		//glTexCoord2f (0.0, 0.0);
		glTexCoord2f (t[6], t[7]);
		//glTexCoord2f (0.25*(date/300%4), 0.0);
		glVertex3f(squareVertices[3].x, squareVertices[3].y, squareVertices[3].z);
		glEnd();

        delete [] t;
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}
};
