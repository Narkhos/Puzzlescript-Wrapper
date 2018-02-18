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
 
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
using namespace std;

#include "deck.hpp"

// class Card

Card::Card(string _name, string _type):name(_name), type(_type)
{
	this->txt_type = NULL; this->txt_name = NULL;
}

// Constructeur par copie
Card::Card(const Card& c):name(c.name),type(c.type)
{
	// Copie des textures des 2 faces de la carte
	this->texFront = c.texFront;
	this->texBack = c.texBack;

	// Copie des coordonnées de texture
	for(int i = 0; i <8;++i)
	{
		this->texCoord[i] = c.texCoord[i];
	}

	// Copie des afficheurs de texte des différentes propriété
	this->txt_type = NULL; this->txt_name = NULL;
	if(c.txt_type) this->txt_type = new GUI_TexteDynamique(*(c.txt_type));
	if(c.txt_name) this->txt_name = new GUI_TexteDynamique(*(c.txt_name));
}

Card::~Card()
{
}

void Card::trace(int numero)
{
	if(numero > 0)
	{
		cout << numero << " - " << this->name << " : " << "type=" << this->type;
	}
	else
	{
		cout  << this->name << " : " << "type=" << this->type;
	}
}

void Card::draw(bool back, float x, float y, float w,float h)
{
	GLfloat vtx1[] =
	{
		x, y, 0,
		x+w, y, 0,
		x+w, y+h, 0,
		x, y+h, 0
	};

	GLfloat col1[] =
	{
		1.0,1.0,1.0, 1.0f,
		1.0,1.0,1.0, 1.0f,
		1.0,1.0,1.0, 1.0f,
		1.0,1.0,1.0, 1.0f
	};

	if(back)
	{
		glBindTexture( GL_TEXTURE_2D, this->texBack );
	}
	else
	{
		glBindTexture( GL_TEXTURE_2D, this->texFront );
	}


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vtx1);
	glTexCoordPointer(2, GL_FLOAT, 0, this->texCoord);
	glColorPointer(4, GL_FLOAT, 0, col1);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	if(!back)
	{
		// Actualisation de l'affichage des caractéristiques de la carte
		// TODO : externaliser de manière à ne mettre la texture à jour que lorsque le nombre de cartes change
		stringstream ss_type;
		ss_type << type;
		txt_type->setTexte(TextManager::StringToWString(ss_type.str()));

		// Affichage du nombre de cartes restantes
		txt_type->draw(x+w-txt_type->w,y,1.0,1.0,1.0);
		txt_name->draw(x,y+h/2,1.0,1.0,1.0);
	}
}

void Card::initGraph(string fileFront, string fileBack, TTF_Font *_font)
{
	SDL_Surface *surface = IMG_Load(fileFront.c_str());

	if(!surface)
	{
		cout << "IMG_Load : " << IMG_GetError() << endl;
	}
	else
	{

		texFront = SurfaceToTexture2(surface);
		SDL_FreeSurface(surface); surface = NULL;
	}

	surface = IMG_Load(fileBack.c_str());

	if(!surface)
	{
		cout << "IMG_Load : " << IMG_GetError() << endl;
	}
	else
	{

		texBack = SurfaceToTexture2(surface);
		SDL_FreeSurface(surface); surface = NULL;
	}

	texCoord[0] = 0.0;
	texCoord[1] = 0.0;

	texCoord[2] = 1.0;
	texCoord[3] = 0.0;

	texCoord[4] = 1.0;
	texCoord[5] = 1.0;

	texCoord[6] = 0.0;
	texCoord[7] = 1.0;

	// Initialisation des afficheurs des différentes statistiques de la carte
	SDL_Color couleurTexte_type; couleurTexte_type.r=0; couleurTexte_type.g=0; couleurTexte_type.b=100; couleurTexte_type.a=255;
	stringstream ss_type;
	ss_type << type;
	txt_type = new GUI_TexteDynamique(TextManager::StringToWString(ss_type.str()), _font, couleurTexte_type, NULL);

	SDL_Color couleurTexte_name; couleurTexte_name.r=0; couleurTexte_name.g=0; couleurTexte_name.b=0; couleurTexte_name.a=255;
	stringstream ss_name;
	ss_name << this->name;
	txt_name = new GUI_TexteDynamique(TextManager::StringToWString(ss_name.str()), _font, couleurTexte_name, NULL);
}

// class Deck

// Constructeur par copie
Deck::Deck(const Deck& d)
{
	cartes = d.cartes;
	rangSelect = d.rangSelect;
}

Deck::Deck()
{
	rangSelect = -1;
}

Deck::Deck(string file)
{
	rangSelect = -1;

	ifstream f(file);
	if(f.is_open())
	{
		Card tmp;
		getline(f, tmp.name, '\t');

		while(!f.eof())
		{

			f >> tmp.type;
			this->cartes.push_back(tmp);
			string retourchariot;
			getline(f, retourchariot);
			getline(f, tmp.name, '\t');
		}

		f.close();
	}
	this->melanger();
}
Deck::~Deck()
{

}

void Deck::melanger()
{
	random_shuffle ( cartes.begin(), cartes.end() );
}

void Deck::trace(bool showPrivate)
{
	if(showPrivate)
	{
		for(int i = 0; i < cartes.size(); ++i)
		{
			cartes[i].trace(i+1);
			cout << endl;
		}
	}
}

Card Deck::pickTop()
{
	Card ret;
	try
	{
		if(cartes.size()>0)
		{
			ret = cartes[0];
			cartes.erase (cartes.begin());
		}
		else
		{
			throw new int( 9 );
		}
	}
	catch(const int * e)
	{
		#ifdef __DEBUG__
			if(*e==9)
			{
				std::cerr << "Impossible de prendre une carte, le deck est vide"<< endl;
			}
		#endif

		throw;
	}
	return ret;
}

Card Deck::pickBottom()
{
	Card ret;
	try
	{
		if(cartes.size()>0)
		{
			ret = cartes[cartes.size()-1];
			cartes.erase (cartes.end());
		}
		else
		{
			throw new int( 9 );
		}
	}
	catch(const int * e)
	{
		#ifdef __DEBUG__
			if(*e==9)
			{
				std::cerr << "Impossible de prendre une carte, le deck est vide"<< endl;
			}
		#endif

		throw;
	}
	return ret;
}

Card Deck::pick(int rank)
{
	Card ret;
	try
	{
		if(cartes.size()>rank)
		{
			ret = cartes[rank];
			cartes.erase (cartes.begin()+rank);
		}
		else
		{
			throw new int( 8 );
		}
	}
	catch(const int * e)
	{
		#ifdef __DEBUG__
			if(*e==8)
			{
				std::cerr << "Impossible de prendre la carte de rang " << rank<< ", le deck ne contient que " << cartes.size() << " cartes." << endl;
			}
		#endif

		throw;
	}
	return ret;
}

Deck Deck::pickHand(const int first, const int number)
{
	Deck ret;

	if(number==0) return ret;

	int last = first + number - 1;

	try
	{
		if(first+number<=cartes.size())
		{
			for(int i = first; i <= last; ++i)
			{
				ret.cartes.push_back(this->pick(first));
			}
		}
		else
		{
			throw new int(8);
		}
	}
	catch(const int * e)
	{
		#ifdef __DEBUG__
			if(*e==8)
			{
				std::cerr << "Impossible de prendre la carte de rang " << last << ", le deck ne contient que " << cartes.size() << " cartes." << endl;
			}
		#endif
		throw;
	}
	return ret;
}

void Deck::draw(float x, float y, float wCard,float hCard, bool back)
{
	for(int i = 0; i < cartes.size(); ++i)
	{
		cartes[i].draw(back, x+i*(wCard+5), y, wCard, hCard);  // 5 correspond à l'écart entre les cartes en pixels
		if(i == this->rangSelect)
		{
			// Dessiner le selecteur de carte
			GLfloat vtx1[] =
			{
				x+i*(wCard+5), y, 0,
				x+wCard+i*(wCard+5), y, 0,
				x+wCard+i*(wCard+5), y+hCard, 0,
				x+i*(wCard+5), y+hCard, 0
			};

			GLfloat col1[] =
			{
				1.0,1.0,1.0, 0.5f,
				1.0,1.0,1.0, 0.5f,
				1.0,1.0,1.0, 0.5f,
				1.0,1.0,1.0, 0.5f
			};

			glBindTexture( GL_TEXTURE_2D, this->texSelect );

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, vtx1);
			glTexCoordPointer(2, GL_FLOAT, 0, this->texCoord);
			glColorPointer(4, GL_FLOAT, 0, col1);

			glDrawArrays(GL_TRIANGLE_FAN,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}
	}
}

int Deck::selectCard(float x, float y, float wCard,float hCard, float mouse_x, float mouse_y)
{
	int rang = -1;

	// Les cartes sont disposées en lignes, on vérifie qu'on a cliqué sur la ligne
	if(mouse_y >= y && mouse_y <= y + hCard)
	{
		if(mouse_x >= x)
		{
			rang = (mouse_x-x)/(wCard+5); // 5 correspond à l'écart enter les cartes en pixels
			if(rang>=cartes.size())
			{
				rang = -1;
			}
		}
	}

	if(rang !=-1) this->rangSelect = rang;
	return rang;
}


void Deck::drawPacked(float x, float y, float w,float h)
{
	GLfloat vtx1[] =
	{
		x, y, 0,
		x+w, y, 0,
		x+w, y+h, 0,
		x, y+h, 0
	};

	GLfloat col1[] =
	{
		1.0,1.0,1.0, 1.0f,
		1.0,1.0,1.0, 1.0f,
		1.0,1.0,1.0, 1.0f,
		1.0,1.0,1.0, 1.0f
	};

	glBindTexture( GL_TEXTURE_2D, this->tex );

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vtx1);
	glTexCoordPointer(2, GL_FLOAT, 0, this->texCoord);
	glColorPointer(4, GL_FLOAT, 0, col1);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Actualisation de l'affichage du nombre de cartes
	// TODO : externaliser de manière à ne mettre la texture à jour que lorsque le nombre de cartes change
	stringstream ss_nbCartes;
	ss_nbCartes << cartes.size();
	txt_nbCartes->setTexte(TextManager::StringToWString(ss_nbCartes.str()));

	// Affichage du nombre de cartes restantes
	txt_nbCartes->draw(x+(w/2)-w*(19.0/100.0),y+(h/2)-h*(14.0/100.0),1.0,1.0,1.0);
}

void Deck::initGraph(string file, TTF_Font *_font)
{
	SDL_Surface *surface = IMG_Load(file.c_str());

	if(!surface)
	{
		cout << "IMG_Load : " << IMG_GetError() << endl;
	}
	else
	{
		this->tex = SurfaceToTexture2(surface);
		SDL_FreeSurface(surface); surface = NULL;
	}

	surface = IMG_Load("images/card_select.png");

	if(!surface)
	{
		cout << "IMG_Load : " << IMG_GetError() << endl;
	}
	else
	{

		this->texSelect = SurfaceToTexture2(surface);
		SDL_FreeSurface(surface); surface = NULL;
	}

	texCoord[0] = 0.0;
	texCoord[1] = 0.0;

	texCoord[2] = 1.0;
	texCoord[3] = 0.0;

	texCoord[4] = 1.0;
	texCoord[5] = 1.0;

	texCoord[6] = 0.0;
	texCoord[7] = 1.0;

	SDL_Color couleurTexte; couleurTexte.r=255; couleurTexte.g=255; couleurTexte.b=255; couleurTexte.a=255;
	stringstream ss_nbCartes;
	ss_nbCartes << cartes.size();
	txt_nbCartes = new GUI_TexteDynamique(TextManager::StringToWString(ss_nbCartes.str()), _font, couleurTexte, NULL);

	// Initialisation des cartes
	// TODO : à remonter au niveau du Player
	for(int i = 0; i < cartes.size(); ++i)
	{
		// Pour le moment, on se contente de mettre des images par défaut à toutes les cartes
		cartes[i].initGraph("images/card_bg.png","images/card_back.png", _font);
	}

}

///
bool sortCard_Croissant (const Card& a,const Card& b) { return (a.name<b.name); }
bool sortCard_Decroissant (const Card& a,const Card& b) { return (a.name>b.name); }
