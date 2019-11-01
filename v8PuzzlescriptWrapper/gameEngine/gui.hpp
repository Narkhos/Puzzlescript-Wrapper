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
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/glew.h>

using namespace std;

#ifndef GUI_WIDGET_UNDEFINED
#define GUI_WIDGET_UNDEFINED 0
#endif

#ifndef GUI_WIDGET_BUTTON
#define GUI_WIDGET_BUTTON 1
#endif

#ifndef GUI_WIDGET_TEXTINPUT
#define GUI_WIDGET_TEXTINPUT 2
#endif

GLuint SurfaceToTexture(SDL_Surface *surface);

Uint16* wstringToUint16(wstring chaine);
std::wstring utf8_to_utf16(const std::string& utf8);
std::string utf16_to_utf8(const std::wstring& utf16);

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, const std::wstring& texte, int &w, int &h);
GLuint texteToTexture(const TTF_Font *font, SDL_Color color, SDL_Color bgColor, const std::wstring& texte, int &w, int &h, Uint32 wrapLength);
GLuint loadImage(string file);
void drawImage(GLuint tex, float x, float y, float w, float h, float alpha, GLfloat luminosite = 1.0, GLfloat texCoord[8] = NULL);

/**
	Zone cliquable à l'écran
*/
class GUI_Widget
{
	public:

	float x; ///< Coordonnée x de la zone cliquable
	float y; ///< Coordonnée y de la zone cliquable
	float w; ///< Largeur de la zone cliquable
	float h; ///< Hauteur de la zone cliquable

	bool visible;
	bool actif;

	wstring txtInfoBulle; ///< Texte à afficher dans l'info bulle

	int widgetType; ///< Type de widget (défini selon chaque classe dérivée)

	string code;	///< Code de référence de l'objet. A renseigner si on à besoin que l'objet puisse être identifié dans une liste par exemple
	/**
		@param _x Coordonnée x de la zone cliquable
		@param _y Coordonnée y de la zone cliquable
		@param _w Largeur de la zone cliquable
		@param _h Hauteur de la zone cliquable
		@param _wodgetType Type de widget
	*/
	GUI_Widget(float _x, float _y, float _w, float _h, int _widgetType);

	~GUI_Widget();

	/**
		Fonction à exécuter lorsque la zone est activé par un clique
	*/
	virtual void action() = 0;

	/**
		Lance l'action liée à la zone cliquable si les coordonnées sont dans la zone
		@param mouseX position x à l'écran
		@param mouseY position y à l'écran
	*/
	bool clique(float mouseX, float mouseY);

	virtual void update();

	// bool rollOver(float mouseX, float mouseY, int scrWidth, int scrHeight);

	bool bCollisionRect(float x2, float y2, float w2, float h2);
	/**
		Change la position de la zone cliquable
	*/
	void setPos(float _x, float _y);

	/**
		Retourne la position x du bord droit de la zone cliquable
		@return position x du bord droit de la zone cliquable
	*/
	float get_x2();

	/**
		Retourne la position y du bord inférieur de la zone cliquable
		@return position y du bord inférieur de la zone cliquable
	*/
	float get_y2();

	/**
		Vérifie que la position est dans la zone cliquable
		@param mouseX position X à vérifier
		@param mouseY position Y à vérifier
		@return renvoie true si mouseX,mouseY est dans la zone cliquable
	*/
	bool isIn(float mouseX, float mouseY);

	virtual void draw() = 0;
};

/**
	Décrit un bouton à plusieurs états (1 à 5 états)
*/
class GUI_Button : public GUI_Widget
{
	public:

	int tex; ///< id de la texture dans le contexte opengl
	int etat; ///< Etat courant du bouton ( 0 : actif; 1 : enfoncé; 2 : grisé; 3 : en cours; 4 : survol
	int nbEtats; ///< Nombre d'états du bouton
	GLfloat texCoord[5][8]; ///< Coordonnées de texture pour chaque état

	/**
		@param file chemin de l'image du bouton
		@param _x position x du bouton à l'écran
		@param _y position y du bouton à l'écran
		@param _w largeur du bouton
		@param _h hauteur du bouton
		@param _nbEtats (entre 1 et 5)
		@param _txtInfoBulle texte de l'info bulle
	*/
	GUI_Button(string file, float _x, float _y, float _w, float _h, int _nbEtats);
	GUI_Button(int _tex, float _x, float _y, float _w, float _h, int _nbEtats);

	~GUI_Button();

	void loadImage(string file);

	/**
		Initialisation des coordonnées de texture
	*/
	void initTexCoord();

	/**
		Dessine le bouton à l'écran
	*/
	virtual void draw();

	/**
		Change l'état du bouton
		@param _etat nouvel état du bouton
		@return true si l'état demandé est valide et a pu être changé
	*/
	bool setEtat(int _etat);

	/**
		Traitement à effectuer au clique sur le bouton
	*/
	virtual void action();
};

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
		Change la couleur du texte
		@param _couleur Nouvelle couleur
	*/
	void setColor(SDL_Color &_couleur);

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

	/**
		Change la couleur du texte de toutes les entries
		@param _couleur Nouvelle couleur
	*/
	void setColor(SDL_Color &_couleur);
};

#endif
