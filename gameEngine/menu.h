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

#ifndef __MENU_H__
#define __MENU_H__

#include <string.h>
#include "gltext2d.hpp"
#include "text_list.hpp"
#include <iostream>
#include <stdlib.h>

using namespace std;

class Menu;

/**
	La classe EntreeMenu est une classe abstraite décrivant un élément de menu.
	Il s'agit donc d'un texte, le libèlé de l'entrée (par exemple : "Jouer").
	Ce texte peut-être affiché dans le canvas OpenGL grâce à la fonction display
	qui utilise pour cela le membre afficheur.
*/
class EntreeMenu
{
	public:

	char *texte;			//!< Libellé de l'entrée du menu.
	int code_texte;

	glText2d *afficheur;	//!< Pointeur vers un afficheur de texte.

	/**
		Pour créer une occurence de EntreeMenu, il est nécessaire d'avoir son
		libèlé ainsi qu'un pointeur vers un afficheur de texte.
	*/
	EntreeMenu(int code_texte, TextList* textlist, glText2d *afficheur)
	{
		this->code_texte=code_texte;
		this->texte=textlist->get_copy(code_texte);
		this->afficheur=afficheur;
	}

	/**
		Affichage de l'entrée à la position pos_x,pos_y sur le canvas OpenGL.
	*/
	virtual void display(int pos_x, int pos_y, float couleur[3])
	{
		afficheur->display(texte,pos_x,pos_y,couleur);
	}

	virtual char* get_text()
	{
		char *c=new char[strlen(texte)+1];
		strcpy(c,texte);
		return c;
	}

	/**
		La fonction actionner défini les actions a effectuer lorsque l'entrée
		est actionnée par l'utilisateur (par exemple, si l'on actionne l'entrée
		"Options", le menu Options s'ouvre). Selon la nature de l'EntreeMenu,
		elle renvoi deux types d'informations différentes :
			* à la sortie de cette fonction, code_fonction contient un code,
			prédéfini
			dans le programme, permettant de déterminer un traitement spécifique
			à effectuer. Par exemple, si l'on a défini une constante
   			CODE_QUITTER égale à 1, il suffit de tester si le code retourné est
   			1. si c'est le cas, cela signifie que l'EntreeMenu actionnée était
      		de type EntreeMenu_execution et que l'action associée était quitter.

			* Le pointeur de retour de type Menu, s'il n'est pas nulle, indique
			que l'on dois changer le menu courrant (c'est le cas de la classe
   			EntreeMenu_sous_menu)

   			* Enfin, dans les cas de EntreeMenu_entier et EntreeMenu_list, ces
   			variables sont toutes deux nulles puisque la fonction actionner
   			ne fait que modifier une variable numérique dont la classe possède
   			un pointeur.

	*/
	virtual Menu* actionner(int& code_fonction, bool avant=true)=0;

	virtual void maj_texte(TextList* textlist)
	{
		delete [] texte;
		texte=NULL;
		texte=textlist->get_copy(code_texte);
		return;
	}
};

/**
	EntreeMenu spécialisée dans l'ouverture d'un autre menu.
*/
class EntreeMenu_sous_menu : public EntreeMenu
{
	public:

	Menu * menu_cible;		/**< Pointeur vers le menu à activer si cette entrée
 							est actionnée*/

	EntreeMenu_sous_menu(int _code_texte, TextList* _textlist, glText2d *afficheur,Menu *_menu_cible)
		:EntreeMenu(_code_texte,_textlist,afficheur),menu_cible(_menu_cible)
			{}

	virtual Menu* actionner(int& code_fonction, bool avant=true)
	{
		code_fonction=0;
		return this->menu_cible;
	}
};

/**
	EntreeMenu spécialisée dans la modification d'un entier.
	Chaque fois que cette entrée est activée, la valeur de la variable
	associée est incrémentée (tout en restant entre min et max. Donc, lorsque
	max est atteint, l'actionner de nouveau fait revenir à min).
*/
template<class T>
class EntreeMenu_valeur : public EntreeMenu
{
	public:

	T *valeur;		//!< Pointeur vers la variable à modifier
	T min;			//!< Valeur minimum de la variable
	T max;			//!< Valeur maximum de la variable
	T step;

	/**
		Le constructeur prend en paramètre un pointeur vers la variable
		que l'EntreeMenu_entier est chargée de modifier.
	*/
	EntreeMenu_valeur(int _code_texte, TextList* _textlist, glText2d *afficheur,T* _valeur
 							,T _min, T _max, T _step)
		:EntreeMenu(_code_texte,_textlist,afficheur),valeur(_valeur),min(_min),max(_max)
		,step(_step)
			{}

	virtual Menu* actionner(int& code_fonction, bool avant=true)
	{
		code_fonction=0;
		if(avant)
		{
			if(*valeur!=max)
			{
				(*valeur)+=step;
				if((*valeur)>max) (*valeur)=max;
			}
			else *valeur=min;
		}
		else
		{
			if(*valeur!=min)
			{
				(*valeur)-=step;
				if((*valeur)<min) (*valeur)=min;
			}
			else *valeur=max;
		}

  		return NULL;
	}

	virtual char* get_text()
	{
		char *txt=new char[256];
		sprintf(txt,"%s %d",texte,*valeur);
		return txt;
	}

	/**
		La fonction display est différente de celle de EntreeMenu car elle
		doit concatenner le libèlé de l'entrée et la valeur de sa variable
  		associée.
	*/
	virtual void display(int pos_x, int pos_y, float couleur[3])
	{
		char txt[256];
		sprintf(txt,"%s %d",texte,*valeur);
		afficheur->display(txt,pos_x,pos_y,couleur);
	}
};

/**
	EntreeMenu_liste est comparable à EntreeMenu_entier, sauf qu'au lieu
	d'afficher directement la valeur numérique modifiée, on affiche un texte
	associé à cette valeur. Cela permet de rendre plus compréhensible la
	signification des valeurs. Par exemple, pour la vitesse d'animation, plutôt
	que d'utiliser une occurence d'EntreeMenu_entier avec les valeurs 1 à 3, on
	utilise EntreeMenu_liste et on associe les libellés : rapide, normale, lente.
*/
template <class T>
class EntreeMenu_liste : public EntreeMenu
{
	public:

	// libell&s et valeurs correspondantes
	bool from_file;				/**< Est à vrai si les libellés viennent du
							fichier. Sinon, cela signifie que les libellés sont
       						les mêmes quelle que soit la langue selectionnée*/
	vector<int> codes_libelles;
	vector<int> codes_fonction;
	vector<char *> libelles;		//!< libellés corespondant à chaque valeur
	vector<T> liste_valeurs;	/**< liste des valeurs que peu prendre la
 									variable */
	int rang_courrant; 			//!< rang de l'option sélectionnée
	T *valeur; 				//!< pointeur sur la variable à modifier

	EntreeMenu_liste(int _code_texte, TextList* _textlist, glText2d *afficheur
						,T* _valeur,bool _from_file=true)
		:EntreeMenu(_code_texte,_textlist,afficheur),valeur(_valeur)
			, rang_courrant(0), from_file(_from_file)
			{}

	~EntreeMenu_liste()
	{
		for(int i=0;i<libelles.size();i++)
		{
			delete [] libelles[i];
			libelles[i]=NULL;
		}
	}

	virtual Menu* actionner(int& code_fonction, bool avant=true)
	{
		if(avant)
		{
			if(rang_courrant!=libelles.size()-1) rang_courrant++;
			else rang_courrant=0;
		}
		else
		{
			if(rang_courrant!=0) rang_courrant--;
			else rang_courrant=libelles.size()-1;
		}
		*valeur=liste_valeurs[rang_courrant];
		code_fonction=this->codes_fonction[rang_courrant];

  		return NULL;
	}

	// détermination du libele courrant en fonction de la valeur courrante
	// cette fonction est utilisée par get_text() lorsque la valeur a été
	// modifiée par une routine extérieure à l'EntreeMenu_liste
	void determine_libele()
	{
		for(rang_courrant=0;rang_courrant<liste_valeurs.size();rang_courrant++)
		{
			if(liste_valeurs[rang_courrant]==*valeur) return;
		}
		rang_courrant=0;
		return;
	}

	virtual char* get_text()
	{
		if(liste_valeurs[rang_courrant]!=*valeur)
		{
			determine_libele();
		}

		char *txt=new char[256];
		sprintf(txt,"%s %s",texte,libelles[rang_courrant]);
		return txt;
	}


	/**
		Affichage de l'entrée en concaténant le texte associé et le libèlé
		correspondant à la valeur courrante.
	*/
	virtual void display(int pos_x, int pos_y, float couleur[3])
	{
		char txt[256];
		sprintf(txt,"%s %s",texte,libelles[rang_courrant]);
		afficheur->display(txt,pos_x,pos_y,couleur);
	}

	/**
		Permet d'ajouter un nouveau couple libellé/valeur dans la liste.
	*/
	void add(int code, TextList *textlist, T valeur, int code_fonction=0)
	{
		if(!from_file)
		{
			cout << "Utilisez ";
   			cout <<" add(char* libelle, T valeur, int code_fonction=0)"<<endl;
   			exit(1);
		}

		this->codes_fonction.push_back(code_fonction);
		codes_libelles.push_back(code);
		libelles.push_back(textlist->get_copy(code));
		liste_valeurs.push_back(valeur);
		set();
	}
	/**
		Fonction d'ajout a utiliser si from_file==false
	*/
	void add(char* libelle, T valeur, int code_fonction=0)
	{
		if(from_file)
		{
			cout << "Utilisez ";
   			cout <<" add(int code, TextList *textlist, T valeur, int code_fonction=0)";
      		cout <<endl;
      		exit(1);
		}

		this->codes_fonction.push_back(code_fonction);
		codes_libelles.push_back(0);
		char *tmp=new char[strlen(libelle)+1];
		strcpy(tmp,libelle);
		libelles.push_back(tmp);
		liste_valeurs.push_back(valeur);
		set();
	}

	/**
		Positionne la liste sur l'élément correspondant à la valeur
		de la variable désignée par "valeur"
	*/
	void set()
	{
				for(int i=0; i< libelles.size();i++)
				{
					if((*(this->valeur))==liste_valeurs[i])
					{
						rang_courrant=i;
						return;
					}
				}

				rang_courrant=0;
	}

	virtual void maj_texte(TextList* textlist)
	{
		delete [] texte;
		texte=NULL;
		texte=textlist->get_copy(code_texte);

		if(from_file==true)
		{
			for(int i=0;i<libelles.size();i++)
			{
				delete [] libelles[i];
				libelles[i]=NULL;
				libelles[i]=textlist->get_copy(codes_libelles[i]);
			}
		}

		return;
	}
};

/**
	EntreeMenu associé à l'exécution d'une fonction. La fonction est désignée
 	par un code_fonction, donné en paramètre au constructeur et renvoyé par
 	actionner(). Le code doit ensuite être utilisé par la fonction appelante
  	pour déterminer la suite à donner.
*/
class EntreeMenu_execution : public EntreeMenu
{
	public:

	int code_fonction;	//!< Code de la fonction liée à l'activation de l'entrée

	EntreeMenu_execution(int _code_texte, TextList* _textlist, glText2d *afficheur,int _code_fonction)
		:EntreeMenu(_code_texte,_textlist,afficheur),code_fonction(_code_fonction)
			{}

	virtual Menu* actionner(int& code_fonction, bool avant=true)
	{
  		code_fonction=this->code_fonction;
  		return NULL;
	}
};

/**
	La classe Menu est une liste d'EntreeMenu. Elle décrit une page de menu.
	Par exemple Menu principal : jouer, options, quitter.
*/
class Menu
{
	public:
	//int pos_x;						//!< Position X du menu à l'écran
	//int pos_y;						//!< Position Y du menu à l'écran
	TxtWindow * txtwindow;
	unsigned int rang_selection;	/**< rang de l'EntreeMenu courrante dans
										 liste_entree*/
	glText2d *gltext2d;				//!< Pointeur vers un afficheur de texte
	float couleur_non_select[3];	//!< Couleur des éléments non selectionnés
	float couleur_select[3];		//!< Couleur de l'élément sélectionné
	char * titre;					//!< Titre du Menu
	int code_titre;
	float couleur_titre[3];			//!< Couleur du titre

	vector<EntreeMenu *> liste_entrees; //!< Liste des entrées du Menu

	/**
		Construction d'un Menu. x,y corespond aux coordonnées écran du coin
		supérieur gauche du menu. Largeur et hauteur corespondent aux dimensions
		de la fenêtre en pixels. On défini 3 couleur : celle du titre, celle
		des éléments non sélectionnés et celle de l'élément selectionné.
	*/
	Menu(int code_titre, TextList* textlist,int x, int y, int largeur,int hauteur, float coul_select[3]
 			, float coul_non_select[3], float coul_titre[3], glText2d* gltext2d)
	{
		txtwindow=new TxtWindow(gltext2d,x,y,largeur,hauteur);

		this->code_titre=code_titre;
		this->titre=textlist->get_copy(code_titre);


		rang_selection=0;

		this->gltext2d=gltext2d;

		for(int i=0;i<3;i++)
		{
			couleur_non_select[i]=coul_non_select[i];
			couleur_select[i]=coul_select[i];
			couleur_titre[i]=coul_titre[i];
		}
	}

	/**
		Destruction du Menu. cette fonction détruit chaque entree de la liste.
	*/
	~Menu()
	{
		for(int i=0;i<liste_entrees.size();i++)
		{
			delete (liste_entrees[i]);
			liste_entrees[i]=NULL;
		}
		delete [] titre;
	}

	/**
		Ajoute une entrée au Menu
	*/
	void ajouter_entree(EntreeMenu* entree)
	{
		liste_entrees.push_back(entree);
	}

	/**
		Affichage du Menu
	*/
	void afficher()
	{
		int tmp=txtwindow->display(titre, 0, 0,couleur_titre)+2;
		for(int i=0;i<liste_entrees.size();i++)
		{
			char * c=liste_entrees[i]->get_text();

			if(i!=rang_selection)
				tmp=txtwindow->display(c, 5, tmp,couleur_non_select)+1;
			else
				tmp=txtwindow->display(c, 5, tmp,couleur_select)+1;

			delete [] c;
		}
	}

	/**
		Selection de l'EntreeMenu suivante
	*/
	void suivant()
	{
		if(rang_selection==liste_entrees.size()-1)
		{
			rang_selection=0;
		}
		else
		{
			rang_selection++;
		}
	}

	/**
		Selection de l'EntreeMenu précédente
	*/
	void precedent()
	{
		if(rang_selection==0)
		{
			rang_selection=liste_entrees.size()-1;
		}
		else
		{
			rang_selection--;
		}
	}

	/**
		Selectionne l'élément de menu pointé par le curseur de souris (x,y).
		Cette fonction renvoi true si le curseur est dans la fenêtre.
		Sinon elle renvoi false.
	*/
	int selectionner(int x, int y)
	{
		int rang=txtwindow->numero_ligne(x,y)-2;
		if(rang>=liste_entrees.size()) return 0;
		if(rang==rang_selection) return 1; // Un point de menu est sélectionné mais c'est le même que précédement

		rang_selection=(rang<0)?rang_selection:rang;
		if(rang<0) return 0;
		return 2; // le point de menu sélectionné n'est pas le même que précédement

	}

	/**
		Actionne l'EntreeMenu courrante. C'est à dire celle qui est désignée
		par rang_selection.
	*/
	virtual Menu* actionner(int& code_fonction, bool avant=true)
	{
		return liste_entrees[rang_selection]->actionner(code_fonction,avant);
	}

	void maj_texte(TextList * textlist)
	{
		delete [] titre;
		titre=NULL;
		titre=textlist->get_copy(this->code_titre);

		for(int i=0;i<liste_entrees.size();i++)
		{
			liste_entrees[i]->maj_texte(textlist);
		}
	}
};

/**
	ListeMenus, comme son nom l'indique, est une liste d'occurences de la
 	classe Menu. Cela permet de gérer une arborescence de menus.
  	C'est ListeMenus qui se charge de traiter les informations de changement
  	de Menu renvoyées par les EntreeMenu_sous_menu.
*/
class ListeMenus
{
	public:

	vector<Menu *> menus;		//!< Liste des menus
	int menu_courrant;			//!< rang du menu courrant

	/**
		Création d'un menu vide
	*/
	ListeMenus()
	{
		menu_courrant=0;
	}

	/**
		Destruction de la liste de menus. Cette fonction détruit chaque menu
		de la liste.
	*/
	~ListeMenus()
	{
		for(int i=0;i<menus.size();i++)
		{
			delete (menus[i]);
			menus[i]=NULL;
		}
	}

	/**
		Ajout d'un Menu dans la liste
	*/
	void add_menu(Menu *menu)
	{
		menus.push_back(menu);
	}

	/**
		Change le menu courrant pour celui correspondant au pointeur donné en
		paramètre.
	*/
	void change_menu_courrant(Menu* menu)
	{
		for(int i=0;i<menus.size();i++)
		{
			if(menus[i]==menu)
			{
				menu_courrant=i;
				return;
			}
		}
	}

	/**
		Retourne un pointeur sur le Menu courrant.
	*/
	Menu* get_menu_courrant()
	{
		return menus[menu_courrant];
	}

	/**
		Affiche le Menu courrant dans l'écran OpenGL
	*/
	void afficher()
	{
		glDisable(GL_DEPTH_TEST);
		menus[menu_courrant]->afficher();
		glEnable(GL_DEPTH_TEST);
	}

	/**
		Selectionne l'EntreeMenu suivante du Menu courrant
	*/
	void element_menu_suivant()
	{
		menus[menu_courrant]->suivant();
	}

	/**
		Selectionne l'EntreeMenu précédente du Menu courrant
	*/
	void element_menu_precedent()
	{
		menus[menu_courrant]->precedent();
	}

	/**
		Actionne l'EntreeMenu courrante du Menu courrant.
		Si la fonction actionner du menu courrant renvoi un pointeur non nulle,
		on change le menu courrant.
		Cette fonction renvoi un entier correspondant au code_fonction renvoyé
		par la fonction actionner du menu courrant.
		code_fonction=0 signifie qu'il n'y a pas de fonction à exécuter.
	*/
	int actionner(bool avant=true)
	{
		Menu* tmp=NULL;
		int code_fonction;
  		tmp=menus[menu_courrant]->actionner(code_fonction,avant);

		if(tmp!=NULL)
		{
			change_menu_courrant(tmp);
		}

		return code_fonction;
	}

	/**
		Selectionne l'élément de menu pointé par le curseur de souris (x,y).
		Cette fonction renvoi true si le curseur est dans la fenêtre.
		Sinon elle renvoi false.

		Elle fait simplement appel à la fonction selectionner du menu courrant.
	*/
	int selectionner(int x, int y)
	{
		return menus[menu_courrant]->selectionner(x,y);
	}

	void maj_texte(TextList * textlist)
	{
		for(int i=0;i<menus.size();i++)
		{
			menus[i]->maj_texte(textlist);
		}
	}
};

#endif
