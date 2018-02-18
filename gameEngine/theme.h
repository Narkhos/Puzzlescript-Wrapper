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

#ifndef __THEME__H__
#define __THEME__H__

#include <dirent.h>
#include <string>
#include "chemins.h"
#include "menu.h"

using namespace std;

class ThemeFileFinder
{
	private:
	vector<string> liste_themes;	//!< Liste des répertoires des themes
	int indice_theme_defaut;	//!< Indice du theme par défaut

	public:
	int indice_theme_courant;	//!< Indice du thème courant dans la liste


	ThemeFileFinder(EntreeMenu_liste<int>* entree_menu=NULL, int code_fonction=0
				,const char* chemin_defaut=THEME_PAR_DEFAUT, const char* repertoire=REP_THEMES, bool is_directory = true)
	{
	    init();
	}

	~ThemeFileFinder()
	{

	}

	void init(EntreeMenu_liste<int>* entree_menu=NULL, int code_fonction=0
				,const char* chemin_defaut=THEME_PAR_DEFAUT, const char* repertoire=REP_THEMES, bool is_directory = true)
	{
		creer_liste_themes(entree_menu, code_fonction, repertoire,is_directory);
		init_indice(chemin_defaut);
	}

	string find(string sous_repertoire
				,string fichier)
	{
		string chemin_complet=liste_themes[indice_theme_courant]+sous_repertoire+fichier;

		ifstream f;
		f.open(chemin_complet.c_str());
		if(f.is_open())
		{
			f.close();
			return chemin_complet;
		}
		else
		{
			return ""; // fichier non trouvé. TODO : déclencher une exception ?
		}
	}

	private:
	void init_indice(string theme)
	{
		for(indice_theme_courant=0;indice_theme_courant<liste_themes.size()
										;indice_theme_courant++)
		{
			if(liste_themes[indice_theme_courant].compare(theme)==0)
			{
				indice_theme_defaut=indice_theme_courant;
				return;
			}
		}
		indice_theme_courant=0;
		indice_theme_defaut=0;
		return;
	}

	void creer_liste_themes(EntreeMenu_liste<int>* entree_menu_theme
		, int code_fonction, string repertoire=REP_THEMES, bool is_directory = true)
	{
		DIR * dir=opendir(repertoire.c_str());
		struct dirent *dirent1;
		int indice=0;
		while((dirent1=readdir(dir))!=0)
		{
			if(strcmp(dirent1->d_name,".")!=0 && strcmp(dirent1->d_name,"..")!=0)
			{
				string nom_theme=string(dirent1->d_name);

					// Stockage du chemin complet du theme dans liste_themes.
					string chemin_theme;
                    if(is_directory)
                    {
                        chemin_theme = repertoire + nom_theme + string("/");
                    }
                    else // si le theme est stockée dans un fichier et non dans un répertoire on n'ajoute pas le / à la fin
                    {
                        chemin_theme = repertoire + nom_theme;
                    }

					liste_themes.push_back(chemin_theme);

					// Création du theme dans le menu.
					char *theme1=new char[strlen(nom_theme.c_str())+1];
					strcpy(theme1,nom_theme.c_str());
					if(entree_menu_theme)
                    {
						entree_menu_theme->add(theme1,indice,code_fonction);
                    }
					indice++;
			}
		}
		closedir(dir);

		return;
	}
public:
    string get_theme_courant()
    {
        return this->liste_themes[this->indice_theme_courant];
    }
};

#endif
