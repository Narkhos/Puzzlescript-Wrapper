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

#ifndef __TEXT_MANAGER__HPP__
#define __TEXT_MANAGER__HPP__

#include <iostream>		// std::cout
#include <string>		// std::string
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

#include "define.hpp"
#include "configSDL.hpp"
#include "chemins.h"

/**
    Liste de textes UNICODE chargée à partir d'un fichier et référencé selon un libellé unique.
    On instancie un TextManager par fichier et par langue que l'on souhaite gérer.

    Le fichier de langue doit être au format UTF-16LE
    Un fichier contient tous les textes pour une langue donnée
    Un texte par ligne. Chaque ligne est de la forme (séparateur : tabulation)
    libelleUniqueDuTexte[TABULATION]Mon texte ici dans la langue du fichier

    Lorsqu'on veut changer la langue de l'application il suffit de passer au constructeur le chemin du fichier
    contenant la langue sélectionnée.
*/
class TextManager
{
	private:
	std::map<wstring,wstring> text;
    string filename;

    public:

	TextManager(ConfigSDL & cfg);
	TextManager(const string & _filename);

	~TextManager();

    void load(const string & _filename);

	wstring getText(wstring libelle);

	static wstring ReadUTF16(const string & filename);

	static Uint16* wstringToUint16(wstring chaine);

	static vector<wstring> wStringTokenizer(wstring& chaine, wchar_t separateur);

	static vector<string> stringTokenizer(string& chaine, char separateur);

	static wstring StringToWString(const std::string& s);

	//static string WStringToString(const std::wstring& ws);

	static std::wstring utf8_to_utf16(const std::string& utf8);
	static std::string utf16_to_utf8(const std::wstring& utf16);
	//static Str TextManager::utf16_to_utf8(const WStr& src)

	static std::istream& safeGetline(std::istream& is, std::string& t);
	static void removeCharsFromString( string &str, string charsToRemove );
	
	static void replaceSubstring(std::string& str, const std::string& pattern, const std::string& rep);
};

#endif
