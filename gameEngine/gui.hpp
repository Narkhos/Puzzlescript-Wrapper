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

#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <iostream>		// std::cout
#include <string>		// std::string
#include <map>
using namespace std;

#include "define.hpp"
#include "display.hpp"
#include "theme.h"
#include "animation.hpp"

#define GUI_WIDGET_UNDEFINED 0
#define GUI_WIDGET_BUTTON 1
#define GUI_WIDGET_TEXTINPUT 2
#define GUI_WIDGET_GAUGE 3

class Engine;
class GUI;

TTF_Font* loadFont(string filepath);

// Gestionnaire de chargement/recherche de police de caractère
class FontPool
{
	protected:
	map<string,TTF_Font*> fontList;
	ThemeFileFinder* file_finder;

    TTF_Font* loadFont(string filepath);
	public:

	FontPool(ThemeFileFinder* _file_finder);
	~FontPool();
	TTF_Font* getFont(const std::string& name);
};


class GUI_InfoBulle;

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
    GUI* gui;

    int widgetType; ///< Type de widget (défini selon chaque classe dérivée)

	string code;	///< Code de référence de l'objet. A renseigner si on à besoin que l'objet puisse être identifié dans une liste par exemple
	/**
		@param _x Coordonnée x de la zone cliquable
		@param _y Coordonnée y de la zone cliquable
		@param _w Largeur de la zone cliquable
		@param _h Hauteur de la zone cliquable
		@param _txtInfoBulle texte de l'info bulle
	*/
	GUI_Widget(float _x, float _y, float _w,float _h, GUI* _gui, int _widgetType, wstring _txtInfoBulle=L"");

	~GUI_Widget();

	/**
		Fonction à exécuter lorsque la zone est activé par un clique
	*/
	virtual void action()=0;

	/**
		Lance l'action liée à la zone cliquable si les coordonnées sont dans la zone
		@param mouseX position x à l'écran
		@param mouseY position y à l'écran
	*/
	bool clique(float mouseX, float mouseY);

    virtual void update();

	bool rollOver(float mouseX, float mouseY, int scrWidth, int scrHeight);

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

	virtual void draw(bool focus)=0;
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
	GUI_Button(string file, float _x, float _y, float _w,float _h, int _nbEtats, GUI* _gui, wstring _txtInfoBulle=L"");
	GUI_Button(int _tex, float _x, float _y, float _w,float _h, int _nbEtats, GUI* _gui, wstring _txtInfoBulle=L"");

	~GUI_Button();

	void loadImage(string file);

	/**
		Initialisation des coordonnées de texture
	*/
	void initTexCoord();

	/**
		Dessine le bouton à l'écran
	*/
	virtual void draw(bool focus=false);

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

class GUI_TextInput : public GUI_Widget
{
    public:

    GUI_TexteDynamique *text; ///< Texte affiché
    wstring value; ///< Texte réel (identique au texte affiché sauf si c'est un mot de passe
    bool password;
    int maxLength;
    Texture* tex;
    Texture* cursor_tex;
    int cursorX;
    int cursorW;
    int cursorH;

    GUI_TextInput(TTF_Font *_font, SDL_Color _couleur, float _x, float _y, float _w,float _h, GUI* _gui, Texture* _tex=NULL, Texture* _cursor_tex=NULL, int _maxLength = -1, bool _password = false);
    GUI_TextInput(TTF_Font *_font, SDL_Color _couleur, float _x, float _y, GUI* _gui, Texture* _tex, Texture* _cursor_tex, int _maxLength, bool _password = false);
    ~GUI_TextInput();

    virtual void draw(bool focus=false);
    virtual void action();
    void updateCursorPosition();

    void popChar();
    void setText(wstring s);
    void appendText(wstring s);
};

class GUI_Gauge : public GUI_Widget
{
	protected:
	unsigned int max;
	unsigned int val;

	Texture* tex_bg;
    Texture* tex_fg;

	public:

	GUI_Gauge(unsigned int _max, float _x, float _y, float _w,float _h, GUI* _gui, Texture* _tex_bg=NULL, Texture* _tex_fg=NULL);
	~GUI_Gauge();
	void init(unsigned int _max, unsigned int _val);
	void setVal(int v);
	void incVal();
	void incVal(unsigned int step);
	void decVal(unsigned int step);
	void decVal();
	void setMax(unsigned int m);
	void incMax();
	void incMax(unsigned int step);
	void decMax();
	unsigned int getVal();
	unsigned int getMax();
	virtual void draw(bool focus=false);
	virtual void action();

};

/* // TODO
class GUI_Selecteur : public GUI_Widget
{
    public:

    vector<wstring>

    GUI_Selecteur();
    ~GUI_Selecteur();

    virtual void draw();
    virtual void action();
};
*/

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

	bool initFromDirectory(string directoryPath, bool sortAlpha=true, bool navigationEntries=false, bool valueWithPath = false);
};

class GUI_InfoBulle : public GUI_TexteDynamique
{
	public:

	float posX;
	float posY;
	bool visible;

	GUI_InfoBulle(TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor=NULL, Uint32 _wrapLength=0);
	~GUI_InfoBulle();

	void draw(float _ratioX=1.0, float _ratioY=1.0, float _alpha = 1.0f);

	/**
		Vérifie que la position est dans la zone cliquable
		@param mouseX position X à vérifier
		@param mouseY position Y à vérifier
		@return renvoie true si mouseX,mouseY est dans la zone cliquable
	*/
	bool isIn(float mouseX, float mouseY, float _ratioX=1.0, float _ratioY=1.0);
};

class GUI
{
    public:
    map<string,GUI_Widget*> widgets;
    GUI_InfoBulle* infoBulle;
    Engine *engine;
    string focusedWidget;
    bool textInputMode;

    GUI(Engine *engine, string _font, SDL_Color _couleur, SDL_Color* _bgColor=NULL, Uint32 _wrapLength=0);
    ~GUI();
    void addWidget(string libelle, GUI_Widget* _widget);
    void draw();
    void update();
    string rollOver();
    bool clique(string libelle, int m_x, int m_y);
    string clique(int m_x, int m_y);

    void startTextInput();
    void stopTextInput();
    unsigned char accentMem; ///< Accent mémorisé lors de la saisie de texte (pour les caractères se saisissant en 2 fois
    GUI_Widget* getFocusedWidget();
    bool setFocusedWidget(string widget);
    bool textInputKeyDown(const SDL_Event &event);
    void textInputProcessing(const SDL_Event &event);
};

#endif
