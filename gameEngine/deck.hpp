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
 
#ifndef __DECK_HPP__
#define __DECK_HPP__

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>

#include "gui.hpp"

using namespace std;

class Card
{
	public:

	string name;
	string type;

	Card(string _name="", string type="");

	/**
		Constructeur par copie
	*/
	Card(const Card& c);

	~Card();

	virtual void trace(int numero=0);

	// Graphisme : TODO classe dérivée ou alors classe "afficheur" qui prendrait la carte à afficher en paramètre ?

	GLuint texFront; ///< id de la texture dans le contexte opengl
	GLfloat texCoord[8]; ///< Coordonnées de texture
	GLuint texBack; ///< id de la texture dans le contexte opengl

	GUI_TexteDynamique* txt_name;
	GUI_TexteDynamique* txt_type;


	virtual void draw(bool back, float x, float y, float w,float h);
	void initGraph(string fileFront, string fileBack, TTF_Font *_font);
};


class Deck
{
	public:
	vector<Card> cartes;

	Deck(const Deck& d);

	// Constructeur par copie
	Deck();

	Deck(string file);
	~Deck();

	void melanger();

	virtual void trace(bool showPrivate);

	Card pickTop();

	Card pickBottom();

	Card pick(int rank);

	Deck pickHand(const int first, const int number);

	// Graphisme : TODO classe dérivée ?

	GLuint tex; ///< id de la texture dans le contexte opengl
	GLfloat texCoord[8]; ///< Coordonnées de texture
	GUI_TexteDynamique* txt_nbCartes;

	int rangSelect;
	GLuint texSelect; ///< id de la texture dans le contexte opengl

	void draw(float x, float y, float wCard,float hCard, bool back);

	void drawPacked(float x, float y, float w,float h);

	void initGraph(string file, TTF_Font *_font);
	int selectCard(float x, float y, float wCard,float hCard, float mouse_x, float mouse_y);
};

///
bool sortCard_Croissant (const Card& a,const Card& b);
bool sortCard_Decroissant (const Card& a,const Card& b);

#endif
