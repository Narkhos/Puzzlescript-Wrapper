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
 
#ifndef __DEFINE_HPP__
#define __DEFINE_HPP__

	// décommenter pour la version Android
	#ifndef GLES_VERSION
	//#define GLES_VERSION 1
	#endif

	// Décommenter pour désactiver les textures
	#ifndef NO_TEXTURES
	//#define NO_TEXTURES
	#endif

	// Décommenter pour plein écran
	#ifndef SET_FULLSCREEN
	//#define SET_FULLSCREEN
	#endif

	// Décommenter pour lancer la version Android sur PC (permet notamment d'identifier si le problème est lié à la plateforme Android ou à du code spécifique erroné)
	#ifndef ON_PC
	//#define ON_PC
	#endif

	// Décommenter pou utiliser le générateur aléatoire de Puyo Puyo Tsu
	#ifndef TSU_SHUFFLE
	#define TSU_SHUFFLE
	#endif

    #include <GL/glew.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_image.h>

	#ifndef GL_BGR
	#define GL_BGR 0x80E0
	#endif

	#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
	#endif

	#ifdef GLES_VERSION
	#ifdef ON_PC
		#include <SDL2/SDL_opengl.h>
	#else
		#include <SDL2/SDL_opengles2.h>
	#endif
	#else
		#include <SDL2/SDL_opengl.h>
	#endif

	#ifdef GLES_VERSION
		#ifdef ON_PC
			#include <GL/gl.h>
		#else
			#include <GLES/gl.h>
			#include <GLES/glext.h>
		#endif

	#else
		#include <GL/gl.h>
		#include <SDL2/SDL_ttf.h>
	#endif

	#ifdef GLES_VERSION
		#ifndef BINARY_VERSION
		#define BINARY_VERSION L"Android"
		#endif
	#else
		#ifndef BINARY_VERSION
		#define BINARY_VERSION L"win32"
		#endif

		#ifndef GL_CLAMP_TO_EDGE
		#define GL_CLAMP_TO_EDGE 0x812F
		#endif
	#endif

	#ifdef GLES_VERSION
		#ifndef ON_PC
			#define GLdouble     GLfloat
			#define GL_CLAMP     GL_CLAMP_TO_EDGE
			#define glClearDepth glClearDepthf
			#define glOrtho      glOrthof
			#define glFrustum    glFrustumf

			#define glColor4fv(a) glColor4f(a[0], a[1], a[2], a[3])
			#define glColor3fv(a) glColor4f(a[0], a[1], a[2], 1.0f)
			#define glColor3f(a,b,c) glColor4f(a, b, c, 1.0f)
		#endif
	#endif
#endif
