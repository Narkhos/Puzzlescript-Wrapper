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

#ifndef __DISPLAY_HPP__
#define __DISPLAY_HPP__
#include "SDL2/SDL.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "configSDL.hpp"
#include "glm/glm.hpp"
#include <iostream>		// std::cout
#include <string>		// std::string
#include <fstream>
#include <sstream>


#include "define.hpp"
#include "textmanager.hpp"

Uint32 get_pixel32( SDL_Surface *surface, int x, int y );

void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel );

/**
	Afficher une image
	@param tex id de la texture
	@param x abscisse à laquelle afficher l'image
	@param y ordonnée à laquelle afficher l'image
	@param w largeur de l'image à l'écran
	@param h hauteur de l'image à l'écran
	@param alpha Niveau de transparence (valeur entre 0.0 et 1.0 avec 1.0 = opaque, 0.0 = invisible)
*/
void drawImage(GLuint tex, float x, float y, float w, float h, float alpha, GLfloat luminosite = 1.0, GLfloat texCoord[8]=NULL);

/**
	Créer une texture opengl à partir d'un texte.
	@param font police utilisée
	@param texte Texte à écrire
	@param w retourne la largeur de la texture obtenue
	@param h retourne la hauteur de la texture obtenue
	@return id de la texture dans le contexte opengl
*/
GLuint texteToTexture(const TTF_Font *font, SDL_Color color, const std::wstring& texte, int &w, int &h);

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, SDL_Color bgColor, const std::wstring& texte, int &w, int &h,Uint32 wrapLength);
/**
	Convertion d'une surface SDL en Texture openGL
	@param surface Surface à convertir
	@return id de la texture openGL générée
*/
GLuint SurfaceToTexture2(SDL_Surface *surface);

class Image
{
    public:
    GLuint tex;

    Image(string path);
    ~Image();
    void draw(float x, float y, float w, float h, float alpha=1.0f, GLfloat luminosite = 1.0, GLfloat texCoord[8]=NULL);
};
/**
	Classe permettant d'initialiser le mode vidéo SDL ainsi qu'OpenGL.
*/
class Display
{
    public:
	glm::mat4 M; // Matrice de projection sauvegardée ici à l'initialisation du display pour d'éventuels calculs ultérieurs
    unsigned int width;			//!< Résolution horizontale d'origine
    unsigned int height;		//!< Résolution verticale d'origine
    unsigned int current_width;			//!< Résolution horizontale courante (suite au redimentionnement de la fenêtre)
    unsigned int current_height;		//!< Résolution verticale courante (suite au redimentionnement de la fenêtre)
    float ratio;				//!< width / height
   	SDL_RendererInfo info;  //!< Informations concernant le mode vidéo
	int flags; 					//!< flags de SDL_SetVideoMode
	SDL_Window* screen;
	SDL_Surface* screenSurface;
    SDL_Renderer *renderer;
    SDL_GLContext glcontext;

    /**
    	Constructeur prenant en paramètre la résolution d'écran demandée.
    */
    Display(string title, unsigned int _width, unsigned int _height);
    Display(string title, ConfigSDL& cfg, unsigned int _native_width=0, unsigned int _native_height=0);

    /**
        Initialisation du contexte SDL2/OpenGL
    */
    void init(string title);

    ~Display();
};
#endif


