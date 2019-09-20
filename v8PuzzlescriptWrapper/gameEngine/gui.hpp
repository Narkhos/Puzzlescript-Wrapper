/* ***********************************************************************************
 * Copyright (C) 2018 Xavier Direz - http://www.LaFaceObscureDuPixel.fr              *
 *                                                                                   *
 * This file is part of PuzzlescriptWrapper.                                         *
 *                                                                                   *
 * PuzzlescriptWrapper is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU Lesser General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * PuzzlescriptWrapper is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU Lesser General Public License for more details.                               *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public License          *
 * along with  this program; If not, see <http://www.gnu.org/licenses/>.             *
 *************************************************************************************/
 
#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <iostream>		// std::cout
#include <string>		// std::string
#include <vector>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/glew.h>

using namespace std;

Uint16* wstringToUint16(wstring chaine);
std::wstring utf8_to_utf16(const std::string& utf8);
std::string utf16_to_utf8(const std::wstring& utf16);

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, const std::wstring& texte, int &w, int &h);
GLuint texteToTexture(const TTF_Font *font, SDL_Color color, SDL_Color bgColor, const std::wstring& texte, int &w, int &h, Uint32 wrapLength);
void drawImage(GLuint tex, float x, float y, float w, float h, float alpha, GLfloat luminosite = 1.0, GLfloat texCoord[8] = NULL);

/**
	Texte affichable sous forme d'une texture en Opengl et modifiable dynamiquement.
	A chaque fois que le texte est modifié, la texture est générée via texteToTexture()
*/
class GUI_TexteDynamique
{
	public:

	wstring texte;		///< Texte a afficher
	TTF_Font *font;		///< Font à utiliser pour écrire le texte
	SDL_Color couleur;	///< Couleur d'affichage du texte
	SDL_Color* bgColor;	///< Couleur de fond
    Uint32 wrapLength;  ///< Longueur d'une ligne pour le retour à la ligne automatique

	int w;				///< Largeur du texte en pixels
	int h;				///< Hauteur du texte en pixels
	GLuint texId;		///< Id de la texture
	bool isSet;			///< La texture est-elle initialisées ?

	/**
		@param _texte Texte a afficher
		@param _font font a utiliser pour l'affichage
		@param _couleur couleur d'affichage du texte
	*/
	GUI_TexteDynamique(wstring _texte, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor = NULL, Uint32 _wrapLength=0);

	~GUI_TexteDynamique();

	/**
		Change le texte à afficher
		@param _texte Nouveau texte à afficher
	*/
	void setTexte(wstring _texte);

	/**
		@param _x Position x ou écrire le texte
		@param _y Position y ou écrire le texte
		@param _ratioX Taux d'étirement sur l'axe des x
		@param _ratioY Taux d'étirement sur l'axe des y
		@param _alpha Transparence 0.0 : invisible / 1.0 : opaque
	*/
	void draw(float _x, float _y, float _ratioX, float _ratioY, float _alpha);

	/**
		@param _x Position x ou écrire le texte
		@param _y Position y ou écrire le texte
		@param _ratio Taux d'étirement (homotétie)
		@param _alpha Transparence 0.0 : invisible / 1.0 : opaque
	*/
	void draw(float _x, float _y, float _ratio=1.0, float _alpha = 1.0f);

	private :

	/**
		Suppression de la texture opengl représentant le texte
	*/
	void delTexture();


	/**
		Génère si nécessaire la texture affichant le texte
	*/
	void updateTexture();
};

class ListEntry : public GUI_TexteDynamique
{
	public:
	string valeur;

	ListEntry(wstring _texte, string _valeur, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor = NULL);

	~ListEntry();
};

bool sortListEntryAlpha (const ListEntry* i, const ListEntry* j);
bool sortListEntryNumeric (const ListEntry* i, const ListEntry* j);

class GUI_List
{
	public:
	vector<ListEntry*> entries;

	ListEntry* selectedEntry;

	float interligne;
	float intercolonne;
	int nb_lignes;

	float pos_x;
	float pos_y;

	TTF_Font *font;
	SDL_Color couleur;
	SDL_Color* bgColor;

	GUI_List(float _pos_x, float _pos_y, float _interligne,	float _intercolonne, int _nb_lignes, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor = NULL);

	~GUI_List();

	void addEntry(wstring _texte, string _valeur);

	ListEntry* selectEntry(int m_x, int m_y);
	ListEntry* setSelectedEntry(int m_x, int m_y);
	ListEntry* setSelectedEntry(string _valeur);
	ListEntry* setNextEntry();
	ListEntry* setPrecEntry();

	void draw(int m_x, int m_y);
	void drawSelected(float _x, float _y);

	bool initFromDirectory(string directoryPath, bool sortAlpha=true, bool navigationEntries=false);

	bool entryExist(string value);
};

#endif
