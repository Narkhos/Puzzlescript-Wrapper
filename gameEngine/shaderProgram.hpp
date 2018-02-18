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
 
#ifndef _SHADER_PROGRAM_HPP_
#define _SHADER_PROGRAM_HPP_


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

class ShaderProgram
{
	public:

	GLuint vertexShader;
	GLuint fragmentShader;

	GLuint shaderProgram;

	ShaderProgram(string vertexShaderPath, string fragmentShaderPath);

	~ShaderProgram();

	void bindShaderProgram();
	static void unbindShaderProgram();

	private:

	string load(string path);

	bool linkShaderProgram();

	bool addVertexShader(string vertexShaderCode);

	bool addFragmentShader(string fragmentShaderCode);
};

#endif
