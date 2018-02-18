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
 
#include "textmanager.hpp"

TextManager::TextManager(ConfigSDL &cfg)
{
    stringstream ss;
    ss << REP_LANGUE << cfg.language;
    this->load(ss.str());
}

TextManager::TextManager(const string & _filename)
{
    this->load(_filename);
}

TextManager::~TextManager()
{
}

void TextManager::load(const string & _filename)
{
    filename = _filename;
	wstring contenu = TextManager::ReadUTF16(filename);

	// On supprime de BOM au début du fichier
	if(contenu.size()>0) contenu.erase(0,1);

	vector<wstring> lignes = TextManager::wStringTokenizer(contenu, '\n');

	for(unsigned int i = 0; i < lignes.size(); ++i)
	{
		vector<wstring> ligne = TextManager::wStringTokenizer(lignes[i],'\t');
		if(ligne.size() > 1)
		{
			// Insertion du texte dans le tableau associatif
			this->text[ligne[0]] = ligne[1];
		}
	}
}

wstring TextManager::getText(wstring libelle)
{
    if(text.count(libelle) == 0 || this->text[libelle]==L"")
    {
        wstringstream ss;
        ss << "< Text '" << libelle << "' not found in '" << StringToWString(this->filename) << "' >";
        return ss.str();
    }

    return this->text[libelle];

}

wstring TextManager::ReadUTF16(const string & filename)
{
	ifstream file(filename.c_str(), std::ios::binary);
	stringstream ss;
	ss << file.rdbuf() << '\0';
	return wstring((wchar_t *)ss.str().c_str());
}

Uint16* TextManager::wstringToUint16(wstring chaine)
{
	Uint16* texte;

    if(chaine.length() > 0)
    {
        texte = new Uint16[chaine.size()+1];
        for(unsigned int i=0; i<chaine.size(); i++)
        {
            texte[i] = chaine[i];
        }
        texte[chaine.size()] = '\0';
    }
    else
    {
        texte = new Uint16[chaine.size()+2];
        texte[0] = ' ';
        texte[1] = '\0';
    }



	return texte;
}

vector<wstring> TextManager::wStringTokenizer(wstring& chaine, wchar_t separateur)
{
	vector<wstring> tokens;
	wstring token;
	std::wistringstream iss(chaine);

	while ( getline(iss, token, separateur) )
	{
	    // On supprime le '\r' éventuel au cas ou le fichier ait été créé sous windows
	    if(token.at(token.size()-1)==L'\r')
        {
            token = token.substr(0, token.size()-1);
        }
		tokens.push_back(token);
	}

	return tokens;
}

vector<string> TextManager::stringTokenizer(string& chaine, char separateur)
{
	vector<string> tokens;
	string token;
	std::istringstream iss(chaine);

	while ( getline(iss, token, separateur) )
	{
		tokens.push_back(token);
	}

	return tokens;
}

wstring TextManager::StringToWString(const std::string& s)
{
		std::wstring temp(s.length(),L' ');
		std::copy(s.begin(), s.end(), temp.begin());
		return temp;
}

/*string TextManager::WStringToString(const std::wstring& ws)
{
    std::stringstream ss;
    ss << ws.c_str();
    return ss.str();
}*/

std::wstring TextManager::utf8_to_utf16(const std::string& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            // throw std::logic_error("not a UTF-8 string");
            return L"not a UTF-8 string";
        }
        else if (ch <= 0xDF)
        {
            uni = ch&0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch&0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch&0x07;
            todo = 3;
        }
        else
        {
            //throw std::logic_error("not a UTF-8 string");
            return L"not a UTF-8 string";
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                return L"not a UTF-8 string";
                //throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                return L"not a UTF-8 string";
                //throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            return L"not a UTF-8 string";
            //throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            return L"not a UTF-8 string";
            //throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

std::string TextManager::utf16_to_utf8(const std::wstring& src)
{
    std::string dest;
	dest.clear();
	for (size_t i = 0; i < src.size(); i++){
		wchar_t w = src[i];
		if (w <= 0x7f)
			dest.push_back((char)w);
		else if (w <= 0x7ff){
			dest.push_back(0xc0 | ((w >> 6)& 0x1f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else if (w <= 0xffff){
			dest.push_back(0xe0 | ((w >> 12)& 0x0f));
			dest.push_back(0x80| ((w >> 6) & 0x3f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else if (w <= 0x10ffff){
			dest.push_back(0xf0 | ((w >> 18)& 0x07));
			dest.push_back(0x80| ((w >> 12) & 0x3f));
			dest.push_back(0x80| ((w >> 6) & 0x3f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else
			dest.push_back('?');
	}
	return dest;
}

std::istream& TextManager::safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}

void TextManager::removeCharsFromString( string &str, string charsToRemove )
{
   for ( unsigned int i = 0; i < charsToRemove.size(); ++i )
   {
      str.erase( std::remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
   }
}

void TextManager::replaceSubstring(std::string& str, const std::string& pattern, const std::string& rep)
{
    std::string::size_type n = 0;
    while ( ( n = str.find( pattern, n ) ) != std::string::npos )
    {
        str.replace( n, pattern.size(), rep );
        n += rep.size();
    }
}
