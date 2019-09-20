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
 
#include "gui.hpp"
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

Uint16* wstringToUint16(wstring chaine)
{
	Uint16* texte;

	if (chaine.length() > 0)
	{
		texte = new Uint16[chaine.size() + 1];
		for (unsigned int i = 0; i < chaine.size(); i++)
		{
			texte[i] = chaine[i];
		}
		texte[chaine.size()] = '\0';
	}
	else
	{
		texte = new Uint16[chaine.size() + 2];
		texte[0] = ' ';
		texte[1] = '\0';
	}



	return texte;
}

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, const std::wstring& texte, int &w, int &h)
{
	SDL_Surface *s = TTF_RenderUNICODE_Blended(const_cast<TTF_Font*>(font), wstringToUint16(texte), color);
	if (!s)
	{
		cout << "Erreur de génération de la texture de texte : " << TTF_GetError() << endl;
		cout << endl;
	}

	GLuint tid = 0;
	GLint ncolors;
	GLenum texture_format;
	// Convert SDL_Surface to OpenGL Texture
	ncolors = s->format->BytesPerPixel;
	if (ncolors == 4) {
		//alpha channel
		if (s->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}
	else {
		if (ncolors == 3) {
			//no alpha channel
			if (s->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}
		else {
			return 0;
		}
	}

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, ncolors, s->w, s->h, 0,
		texture_format, GL_UNSIGNED_BYTE, s->pixels);

	// On retourne les dimensions de la texture créée
	w = s->w;
	h = s->h;

	SDL_FreeSurface(s);
	s = nullptr;
	return tid;
}

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, SDL_Color bgColor, const std::wstring& texte, int &w, int &h, Uint32 wrapLength)
{
	SDL_Surface *t;
	if (wrapLength > 0)
	{
		t = TTF_RenderUNICODE_Blended_Wrapped(const_cast<TTF_Font*>(font), wstringToUint16(texte), color, wrapLength);
	}
	else
	{
		t = TTF_RenderUNICODE_Blended(const_cast<TTF_Font*>(font), wstringToUint16(texte), color);
	}

	SDL_Surface *s = SDL_CreateRGBSurface(0, t->w + 4, t->h + 4, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_SetSurfaceBlendMode(s, SDL_BLENDMODE_BLEND);


	SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, bgColor.r, bgColor.g, bgColor.b, bgColor.a));


	SDL_Rect cible;
	cible.x = 2; cible.y = 2; cible.w = t->w; cible.h = t->h;

	SDL_BlitSurface(t, NULL, s, &cible);


	GLuint tid = 0;
	GLint ncolors;
	GLenum texture_format;
	// Convert SDL_Surface to OpenGL Texture
	ncolors = s->format->BytesPerPixel;
	if (ncolors == 4) {
		//alpha channel
		if (s->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}
	else {
		if (ncolors == 3) {
			//no alpha channel
			if (s->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}
		else {
			return 0;
		}
	}

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, ncolors, s->w, s->h, 0,
		texture_format, GL_UNSIGNED_BYTE, s->pixels);

	// On retourne les dimensions de la texture créée
	w = s->w;
	h = s->h;

	SDL_FreeSurface(s);
	s = NULL;

	return tid;
}

void drawImage(GLuint tex, float x, float y, float w, float h, float alpha, GLfloat luminosite, GLfloat texCoord[8])
{
	GLfloat vtx1[] =
	{
		x, y, 0.0,
		x + w, y, 0.0,
		x + w, y + h, 0.0,
		x, y + h, 0.0
	};

	GLfloat texCoord1[] =
	{
		0.0,0.0,
		1.0,0.0,
		1.0,1.0,
		0.0,1.0
	};

#ifdef NO_TEXTURES
	// Gestion des couleurs unies dans le cas ou les textures ne fonctionnent pas
	float c_r, c_v, c_b;
	if (tex == tex_r) { c_r = 1; c_v = 0; c_b = 0; }
	if (tex == tex_v) { c_r = 0; c_v = 1; c_b = 0; }
	if (tex == tex_b) { c_r = 0; c_v = 0; c_b = 1; }
	if (tex == tex_j) { c_r = 1; c_v = 1; c_b = 0; }
	if (tex == tex_o) { c_r = 1; c_v = 1; c_b = 1; }

	GLfloat col1[] =
	{
		c_r, c_v, c_b, alpha,
		c_r, c_v, c_b, alpha,
		c_r, c_v, c_b, alpha,
		c_r, c_v, c_b, alpha
	};
	//

#else
	GLfloat col1[] =
	{
		luminosite,luminosite,luminosite, alpha,
		luminosite,luminosite,luminosite, alpha,
		luminosite,luminosite,luminosite, alpha,
		luminosite,luminosite,luminosite, alpha
	};
#endif


	glBindTexture(GL_TEXTURE_2D, tex);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vtx1);
	if (texCoord != NULL)
	{
		glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, texCoord1);
	}


	glColorPointer(4, GL_FLOAT, 0, col1);


	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

std::wstring utf8_to_utf16(const std::string& utf8)
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
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
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

std::string utf16_to_utf8(const std::wstring& src)
{
	std::string dest;
	dest.clear();
	for (size_t i = 0; i < src.size(); i++) {
		wchar_t w = src[i];
		if (w <= 0x7f)
			dest.push_back((char)w);
		else if (w <= 0x7ff) {
			dest.push_back(0xc0 | ((w >> 6) & 0x1f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else if (w <= 0xffff) {
			dest.push_back(0xe0 | ((w >> 12) & 0x0f));
			dest.push_back(0x80 | ((w >> 6) & 0x3f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else if (w <= 0x10ffff) {
			dest.push_back(0xf0 | ((w >> 18) & 0x07));
			dest.push_back(0x80 | ((w >> 12) & 0x3f));
			dest.push_back(0x80 | ((w >> 6) & 0x3f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else
			dest.push_back('?');
	}
	return dest;
}

// class GUI_TexteDynamique

GUI_TexteDynamique::GUI_TexteDynamique(wstring _texte, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor,Uint32 _wrapLength)
    : texte(_texte),font(_font),couleur(_couleur), wrapLength(_wrapLength),
        bgColor(_bgColor), w(0),h(0), texId(0), isSet(false)
{

}

GUI_TexteDynamique::~GUI_TexteDynamique()
{
	delTexture();
}

void GUI_TexteDynamique::setTexte(wstring _texte)
{
    if(texte != _texte)
    {
        texte = _texte;

        // Suppression de la précédente texture s'il y en a une
        delTexture();
    }
}

void GUI_TexteDynamique::draw(float _x, float _y, float _ratioX, float _ratioY, float _alpha)
{
	// génèration de la texture si le texte a changé
	updateTexture();

	// Affichage du texte aux coordonnées demandées
	drawImage(texId, _x, _y, _ratioX * (float)w, _ratioY * (float)h, _alpha);
}

void GUI_TexteDynamique::draw(float _x, float _y, float _ratio, float _alpha)
{
	draw(_x,_y,_ratio,_ratio,_alpha);
}

void GUI_TexteDynamique::delTexture()
{
	if(isSet)
	{
		glDeleteTextures(1,&texId);
		isSet = false;
	}
}

void GUI_TexteDynamique::updateTexture()
{
	if(!isSet)
	{
		// Création de la nouvelle texture
		//texId = texteToTexture(font, couleur, texte, w, h);
		if(this->bgColor != NULL)
		{
			texId = texteToTexture(font, couleur, *(this->bgColor), texte, w, h, this->wrapLength);
		}
		else
		{
			texId = texteToTexture(font, couleur, texte, w, h);
		}
		isSet = true;
	}
}

// class ListEntry : public GUI_TexteDynamique
ListEntry::ListEntry(wstring _texte, string _valeur, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor)
	:valeur(_valeur), GUI_TexteDynamique( _texte, _font, _couleur, _bgColor, 0)
{
}

ListEntry::~ListEntry()
{
}

bool sortListEntryAlpha (const ListEntry* i, const ListEntry* j)
{
    return (i->texte<j->texte);
}

bool sortListEntryNumeric (const ListEntry* i, const ListEntry* j)
{
    return (atoi(utf16_to_utf8(i->texte).c_str())<atoi(utf16_to_utf8(j->texte).c_str()));
}

// class GUI_List : public GUI_Widget
GUI_List::GUI_List(float _pos_x, float _pos_y, float _interligne,	float _intercolonne, int _nb_lignes, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor)
	: pos_x(_pos_x), pos_y(_pos_y), interligne(_interligne), intercolonne(_intercolonne), nb_lignes(_nb_lignes), font(_font),couleur(_couleur), bgColor(_bgColor)
{
    selectedEntry = NULL;
}

GUI_List::~GUI_List()
{
	for(int i = (int)entries.size() - 1; i >=0; --i)
	{
		delete entries[i];
		entries[i] = NULL;
	}
}

void GUI_List::addEntry(wstring _texte, string _valeur)
{
	entries.push_back(new ListEntry(_texte, _valeur,font,couleur,bgColor));
/*	if(entries.size()==1)
    {
        selectedEntry = entries[0];
    }
*/
}

ListEntry* GUI_List::selectEntry(int m_x, int m_y)
{
    int x = (int)(m_x - pos_x);
    int y = (int)(m_y - pos_y);
    if(x<0 || y <0) return NULL;

	int colonne = (int)(x / intercolonne);
	int ligne = (int)( y / interligne);

    if(colonne >= 0 && ligne >= 0 && colonne < entries.size()/nb_lignes+1 && ligne < nb_lignes)
    {
        int rang = colonne*nb_lignes+ligne;
        if(rang>=0 && rang < entries.size()) return entries[rang];
    }

	return NULL;
}

ListEntry* GUI_List::setSelectedEntry(int m_x, int m_y)
{
	selectedEntry = selectEntry(m_x, m_y);

    return selectedEntry;
}

ListEntry* GUI_List::setSelectedEntry(string _valeur)
{
    for(int i = 0; i < entries.size(); ++i)
    {
        if(entries[i]->valeur == _valeur)
        {
            selectedEntry = entries[i];
            return selectedEntry;
        }
    }

    selectedEntry = NULL;
    return NULL;
}

ListEntry* GUI_List::setNextEntry()
{
    if(selectedEntry == NULL)
    {
        if(entries.size()==0)
        {
            return NULL;
        }
        else
        {
            selectedEntry = entries[0];
            return selectedEntry;
        }
    }

    if(entries.size()>1)
    {
        if(selectedEntry == entries[entries.size()-1])
        {
            selectedEntry = entries[0];
            return selectedEntry;
        }

        for(int i = 0; i< entries.size()-1;++i)
        {
            if(selectedEntry == entries[i])
            {
                selectedEntry = entries[i+1];
                return selectedEntry;
            }
        }
    }

    return selectedEntry;
}

ListEntry* GUI_List::setPrecEntry()
{
    if(selectedEntry == NULL)
    {
        if(entries.size()==0)
        {
            return NULL;
        }
        else
        {
            selectedEntry = entries[0];
            return selectedEntry;
        }
    }

    if(entries.size()>1)
    {
        if(selectedEntry == entries[0])
        {
            selectedEntry = entries[entries.size()-1];
            return selectedEntry;
        }

        for(int i = 1; i< entries.size();++i)
        {
            if(selectedEntry == entries[i])
            {
                selectedEntry = entries[i-1];
                return selectedEntry;
            }
        }
    }

    return selectedEntry;
}

void GUI_List::draw(int m_x, int m_y)
{
    ListEntry* tmp = selectEntry(m_x,m_y);

	for(int i = 0; i < entries.size(); ++i)
	{
		int ligne = i%nb_lignes;
		int colonne = i/nb_lignes;

		float x = pos_x + (float)colonne*intercolonne;
		float y = pos_y + (float)ligne*interligne;

        float zoom = 1.0f;
        float alpha = 0.8f;
        if(tmp!=NULL && entries[i] == tmp) zoom = 1.5f;
        if(entries[i] == selectedEntry)
        {
            zoom = 1.2f;
            alpha = 1.0f;
        }
		entries[i]->draw(x, y, zoom, zoom, alpha);
	}
}

void GUI_List::drawSelected(float _x, float _y)
{
    if(selectedEntry != NULL)
    {
        selectedEntry->draw(_x,_y,1.0,1.0,1.0);
    }
}

bool GUI_List::initFromDirectory(string directoryPath, bool sortAlpha,bool navigationEntries)
{
    entries.erase(entries.begin(), entries.end());

	for (const auto & entry : fs::directory_iterator(directoryPath))
	{
		addEntry(fs::path(entry.path()).filename().wstring(), entry.path().filename().string());
	}

    if(sortAlpha)
    {
        std::sort (entries.begin(), entries.end(), sortListEntryAlpha);
    }
    else
    {
        std::sort (entries.begin(), entries.end(), sortListEntryNumeric);
    }

    return true;
}

bool GUI_List::entryExist(string value)
{
	for (int i = 0; i < entries.size(); ++i)
	{
		if (entries[i]->valeur == value)
		{
			return true;
		}
	}

	return false;
}