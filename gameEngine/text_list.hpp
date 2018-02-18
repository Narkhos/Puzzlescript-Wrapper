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

#ifndef __TEXT_LIST_HPP__
#define __TEXT_LIST_HPP__

#include<fstream>
#include<vector>

using namespace std;

struct TextNode
{
	int code;
	char *texte;
};

class TextList
{
	protected:
	vector<TextNode *> liste;

	public:

	TextList(string fichier="")
	{
		if(fichier.compare("")!=0)
		{
			this->load(fichier);
		}
	}

	~TextList()
	{
		this->unload();
	}

	void unload()
	{
		for(int i=0;i<liste.size();i++)
		{
			delete [] liste[i]->texte;
			delete liste[i];
			liste[i]=NULL;
		}
		liste.clear();
	}

	void load(string fichier)
	{
		if(liste.size()>0) unload();

		char tmp[256];
		char c;
		TextNode *noeud=NULL;
		ifstream f;
		f.open(fichier.c_str());

		while(!f.eof())
		{
			noeud=new TextNode();
			f>>noeud->code;
			f>>c;

			f.getline(tmp,255,'\n');
			noeud->texte=new char[strlen(tmp)+1];
			strcpy(noeud->texte,tmp);

			liste.push_back(noeud);
		}
		f.close();
	}

	char * get(int code)
	{
		for(int i=0;i<liste.size();i++)
		{
			if(liste[i]->code==code) return liste[i]->texte;
		}

		return NULL;
	}

	char * get_copy(int code)
	{
		for(int i=0;i<liste.size();i++)
		{
			if(liste[i]->code==code)
			{
				char *tmp=new char[strlen(liste[i]->texte)+1];
				strcpy(tmp,liste[i]->texte);
				return tmp;
			}
		}

		return NULL;
	}
};

#endif
