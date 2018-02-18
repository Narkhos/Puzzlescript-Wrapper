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
 
#include "shaderProgram.hpp"

// class ShaderProgram
ShaderProgram::ShaderProgram(string vertexShaderPath, string fragmentShaderPath)
{
	vertexShader = -1;
	fragmentShader = -1;
	shaderProgram = -1;

	string vertexShaderCode = load(vertexShaderPath);

	if(vertexShaderCode!="")
	{
        addVertexShader(vertexShaderCode);
    }

	string fragmentShaderCode = load(fragmentShaderPath);
	if(fragmentShaderCode!="")
	{
        addFragmentShader(fragmentShaderCode);
    }

	linkShaderProgram();
}

ShaderProgram::~ShaderProgram()
{
	if(vertexShader>=0) glDeleteShader(vertexShader);
	if(fragmentShader>=0) glDeleteShader(fragmentShader);
	if(shaderProgram>=0) glDeleteProgram(shaderProgram);
}

string ShaderProgram::load(string path)
{
    if(path=="") return "";
	string str="";
	ifstream f(path);
	if(f.is_open())
	{
		str = string((std::istreambuf_iterator<char>(f)),
			 std::istreambuf_iterator<char>());
			 f.close();
	}

	return str;
}

bool ShaderProgram::linkShaderProgram()
{
	shaderProgram = glCreateProgram();
	if(vertexShader>=0)
	{
        glAttachShader(shaderProgram, vertexShader);
    }

	if(fragmentShader>=0)
	{
        glAttachShader(shaderProgram, fragmentShader);
	}


	glLinkProgram(shaderProgram);
	// Check for linking errors
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}

void ShaderProgram::unbindShaderProgram()
{
	glUseProgram(0);
}

void ShaderProgram::bindShaderProgram()
{
	if(shaderProgram>=0)
	{
		glUseProgram(shaderProgram);
	}
}

bool ShaderProgram::addVertexShader(string vertexShaderCode)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * tmp = vertexShaderCode.c_str();

	glShaderSource(vertexShader, 1, &tmp, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	delete [] tmp;
	return success;
}

bool ShaderProgram::addFragmentShader(string fragmentShaderCode)
{
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar * tmp = fragmentShaderCode.c_str();

	glShaderSource(fragmentShader, 1, &tmp, NULL);
	glCompileShader(fragmentShader);

	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

    delete [] tmp;
	return success;
}

