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
 
 #include "engine.hpp"
 #include "gui.hpp"


TTF_Font* loadFont(string filepath)
{
    // INITIALISATION de SDL_ttf
	if(!TTF_WasInit() && TTF_Init()==-1)
	{
		cerr << "TTF_Init : " << TTF_GetError() << endl;
		return NULL;
	}

    TTF_Font* font = TTF_OpenFont(filepath.c_str(), 16);

	if(!font)
	{
		cerr << "TTF_OpenFont : " << TTF_GetError()<<endl;
		return NULL;
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	return font;
}

// class FontPool

TTF_Font* FontPool::loadFont(string filepath)
{
        // INITIALISATION de SDL_ttf
	if(!TTF_WasInit() && TTF_Init()==-1)
	{
		cerr << "TTF_Init : " << TTF_GetError() << endl;
		return NULL;
	}

    TTF_Font* font = TTF_OpenFont(filepath.c_str(), 16);

	if(!font)
	{
		cerr << "TTF_OpenFont : " << TTF_GetError()<<endl;
		return NULL;
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	return font;
}

FontPool::FontPool(ThemeFileFinder* _file_finder):file_finder(_file_finder)
{
}

FontPool::~FontPool()
{
	map<string,TTF_Font*>::iterator p;
    for(p = fontList.begin(); p != fontList.end(); p++)
    {
        if(p->second != NULL) TTF_CloseFont(p->second);
        p->second = NULL;

    }
}

TTF_Font* FontPool::getFont(const std::string& fichier)
{
    if(fontList.count(fichier)==0)
    {
        fontList[fichier] = this->loadFont(string(file_finder->find(REP_FONTS,fichier.c_str())));
    }

    return fontList[fichier];
}


// class Cliquable

GUI_Widget::GUI_Widget(float _x, float _y, float _w,float _h, GUI* _gui, int _widgetType, wstring _txtInfoBulle)
    :x(_x),y(_y),w(_w),h(_h), gui(_gui), txtInfoBulle(_txtInfoBulle), code(""), widgetType(_widgetType)
{

    visible = true;
    actif = true;
}

GUI_Widget::~GUI_Widget() {}



bool GUI_Widget::clique(float mouseX, float mouseY)
{
	if(visible && actif && isIn(mouseX, mouseY))
	{
		//this->action();
		return true;
	}

	return false;
}

bool GUI_Widget::rollOver(float mouseX, float mouseY, int scrWidth, int scrHeight)
{
	if(isIn(mouseX, mouseY))
	{

		if(gui->infoBulle != NULL)
		{
			gui->infoBulle->setTexte(txtInfoBulle);
			gui->infoBulle->visible = true;
			gui->infoBulle->posX = max(0.0f,mouseX - (gui->infoBulle->w/2));
			gui->infoBulle->posY = max(0.0f,mouseY - gui->infoBulle->h - 2);

			if(gui->infoBulle->posX + gui->infoBulle->w > scrWidth) gui->infoBulle->posX = scrWidth - gui->infoBulle->w;
			if(gui->infoBulle->posY + gui->infoBulle->h > scrHeight) gui->infoBulle->posY = scrHeight - gui->infoBulle->h;
		}
		return true;
	}

	return false;
}

bool GUI_Widget::bCollisionRect(float x2, float y2, float w2, float h2)
{
	if(x < x2 + w2 &&
	x + w > x2 &&
	y < y2 + h2 &&
	h + y > y2)
	{
		return true;
	}
	return false;
}

void GUI_Widget::update()
{
    // Par défaut, la fonction ne fait rien
}

void GUI_Widget::setPos(float _x, float _y)
{
	x = _x;
	y = _y;
}

float GUI_Widget::get_x2()
{
	return x + w;
}


float GUI_Widget::get_y2()
{
	return y + h;
}

bool GUI_Widget::isIn(float mouseX, float mouseY)
{
	if(mouseX >= x && mouseX <x+w && mouseY >=y && mouseY < y+h)
	{
		return true;
	}
	else
	{
		return false;
	}
}


// class Button : public Cliquable
GUI_Button::GUI_Button(int _tex, float _x, float _y, float _w,float _h, int _nbEtats, GUI* _gui, wstring _txtInfoBulle)
	:GUI_Widget(_x,_y,_w,_h, _gui, GUI_WIDGET_BUTTON, _txtInfoBulle),etat(0), nbEtats(_nbEtats)
{
		this->tex = _tex;
		initTexCoord();
}

GUI_Button::GUI_Button(string file, float _x, float _y, float _w,float _h, int _nbEtats, GUI* _gui, wstring _txtInfoBulle)
	:GUI_Widget(_x,_y,_w,_h,_gui, GUI_WIDGET_BUTTON, _txtInfoBulle),etat(0), nbEtats(_nbEtats)
{
    this->tex = -1;
    loadImage(file);
}

void GUI_Button::loadImage(string file)
{
    SDL_Surface *surface = IMG_Load(file.c_str());

	if(!surface)
	{
		cout << "IMG_Load : " << IMG_GetError() << endl;
	}
	else
	{
        if(tex>-1)
        {
            glDeleteTextures(1,(GLuint*)&(this->tex));
        }

		tex = SurfaceToTexture2(surface);
		SDL_FreeSurface(surface); surface = NULL;

		initTexCoord();
	}
}

GUI_Button::~GUI_Button()
{

}

void GUI_Button::initTexCoord()
{
	// Parcours des 5 états
	for(int e = 0; e < nbEtats; ++e)
	{
		float offset = (float)e / (float)nbEtats;

		texCoord[e][0] = offset;
		texCoord[e][1] = 0.0;

		texCoord[e][2] = offset + 1.0/(float)nbEtats;
		texCoord[e][3] = 0.0;

		texCoord[e][4] = offset + 1.0/(float)nbEtats;
		texCoord[e][5] = 1.0;

		texCoord[e][6] = offset;
		texCoord[e][7] = 1.0;
	}
}

void GUI_Button::draw(bool focus)
{
    if(!visible) return;

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
	glTexCoordPointer(2, GL_FLOAT, 0, this->texCoord[etat]);
	glColorPointer(4, GL_FLOAT, 0, col1);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

bool GUI_Button::setEtat(int _etat)
{
	if(_etat>=0 && _etat < this->nbEtats)
	{
		etat = _etat;
		return true;
	}

	return false;
}

/**
	Traitement à effectuer au clique sur le bouton
*/
void GUI_Button::action()
{
	setEtat((this->etat+1)%nbEtats);
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

// class GUI_InfoBulle

GUI_InfoBulle::GUI_InfoBulle(TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor, Uint32 _wrapLength)
    :GUI_TexteDynamique(L"", _font, _couleur, _bgColor, _wrapLength), posX(0), posY(0), visible(false)
{
}

GUI_InfoBulle::~GUI_InfoBulle()
{
}

void GUI_InfoBulle::draw(float _ratioX, float _ratioY, float _alpha)
{
	if(visible && texte != L"")
	{
		GUI_TexteDynamique::draw(posX, posY, _ratioX, _ratioY, _alpha);
		visible = false;
	}
}

bool GUI_InfoBulle::isIn(float mouseX, float mouseY, float _ratioX, float _ratioY)
{
	if(mouseX >= posX && mouseX <posX+w*_ratioX && mouseY >=posY && mouseY < posY+h*_ratioY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// class GUI_TextInput : public GUI_Widget

GUI_TextInput::GUI_TextInput(TTF_Font *_font, SDL_Color _couleur, float _x, float _y, float _w,float _h, GUI* _gui, Texture* _tex, Texture* _cursor_tex, int _maxLength, bool _password)
    :GUI_Widget(_x,_y,_w,_h,_gui,GUI_WIDGET_TEXTINPUT,L""), tex(_tex), cursor_tex(_cursor_tex), maxLength(_maxLength), password(_password)
{
    text = new GUI_TexteDynamique(L"",_font,_couleur, NULL);

    updateCursorPosition();
}

GUI_TextInput::GUI_TextInput(TTF_Font *_font, SDL_Color _couleur, float _x, float _y, GUI* _gui, Texture* _tex, Texture* _cursor_tex, int _maxLength, bool _password)
    :GUI_Widget(_x,_y,10,10,_gui,GUI_WIDGET_TEXTINPUT,L""), tex(_tex), cursor_tex(_cursor_tex), maxLength(_maxLength), password(_password)
{
    string loginSize = string(_maxLength, 'M');
    int _w;
    int _h;
    if(TTF_SizeText(_font,loginSize.c_str(),&_w,&_h)==0)
    {
        this->w = _w;
        this->h = _h;
    }

    this->cursorW = 10;

    TTF_SizeText(_font, "M",&this->cursorW,&this->cursorH);

    text = new GUI_TexteDynamique(L"",_font,_couleur, NULL);

    updateCursorPosition();
}

GUI_TextInput::~GUI_TextInput()
{
    delete text;
    text = NULL;
}

void GUI_TextInput::draw(bool focus)
{
    if(tex!=NULL) drawImage(tex->getId(), this->x, this->y, this->w, this->h, 1.0);

    if(focus)
    {
        // TODO : dessiner un curseur clignotant à la fin du texte
        if((this->gui->engine->currentTime/500)%2)
        {
            if(this->cursor_tex!=NULL) drawImage(cursor_tex->getId(), this->x+this->cursorX, this->y, this->cursorW, this->cursorH, 1.0);
        }
    }

    text->draw(this->x, this->y,1.0,1.0);
}

void GUI_TextInput::action()
{
}

void GUI_TextInput::updateCursorPosition()
{
    if(this->value.size()==0)
    {
        this->cursorX=0;
    }
    else
    {
        int cursorY;
        string textSize = string(this->value.size(), 'M');
        TTF_SizeText(text->font,textSize.c_str(),&this->cursorX,&cursorY);
    }
}

void GUI_TextInput::popChar()
{
    if(this->value.length() > 0)
    {
        this->value = this->value.substr(0, this->value.size()-1);
        if(this->password)
        {
            this->text->setTexte(wstring(this->value.length(),L'*'));
        }
        else
        {
            this->text->setTexte(this->value);
        }

        updateCursorPosition();
    }
}

void GUI_TextInput::setText(wstring s)
{
    if(maxLength == -1 || s.length() <= maxLength)
    {
        this->value = s;
    }
    else
    {
        this->value = s.substr (0,maxLength);
    }

    if(this->password)
    {
        this->text->setTexte(wstring(this->value.length(),L'*'));
    }
    else
    {
        this->text->setTexte(this->value);
    }

    updateCursorPosition();
}

void GUI_TextInput::appendText(wstring s)
{
    this->value += s;

    if(maxLength != -1 && this->value.length() > maxLength)
    {
        this->value = this->value.substr (0,maxLength);
    }

    if(this->password)
    {
        this->text->setTexte(wstring(this->value.length(),L'*'));
    }
    else
    {
        this->text->setTexte(this->value);
    }

    updateCursorPosition();
}

// class GUI_Gauge : public GUI_Widget
GUI_Gauge::GUI_Gauge(unsigned int _max, float _x, float _y, float _w,float _h, GUI* _gui, Texture* _tex_bg, Texture* _tex_fg)
	:GUI_Widget(_x,_y,_w,_h,_gui,GUI_WIDGET_GAUGE,L""), max(_max), val(_max),tex_bg(_tex_bg),tex_fg(_tex_fg)
{
}

GUI_Gauge::~GUI_Gauge()
{
}

void GUI_Gauge::init(unsigned int _max, unsigned int _val)
{
	max=_max;
	val=_val;
}

void GUI_Gauge::setVal(int v)
{
	val = (v<max)?v:max;
}

void GUI_Gauge::incVal()
{
	val++;
	val = (val<max)?val:max;
}

void GUI_Gauge::incVal(unsigned int step)
{
	val+=step;
	val = (val<max)?val:max;
}

void GUI_Gauge::decVal()
{
	if(val > 0) val--;
}

void GUI_Gauge::decVal(unsigned int step)
{
	if(val < step)
	{
		val = 0;
	}
	else
	{
		val-=step;
	}
}

void GUI_Gauge::setMax(unsigned int m)
{
	max = m;
	val = (val<max)?val:max;
}

void GUI_Gauge::incMax()
{
	max++;
}

void GUI_Gauge::incMax(unsigned int step)
{
	max+=step;
}

void GUI_Gauge::decMax()
{
	max --;
}

unsigned int GUI_Gauge::getVal()
{
	return val;
}

unsigned int GUI_Gauge::getMax()
{
	return max;
}

void GUI_Gauge::draw(bool focus)
{
	if(this->tex_bg != NULL)
	{
		drawImage(tex_bg->getId(), x, y, w, h, 1.0);
	}

	if(this->tex_bg != NULL)
	{
		GLfloat texCoord[8] = {
			0.0, 0.0,
			(float)val/(float)this->max , 0.0,
			(float)val/(float)this->max , 1.0,
			0.0, 1.0

		};
		drawImage(tex_fg->getId(), x, y, (float)w * ((float)val/(float)this->max), h, 1.0, 1.0, texCoord);
	}
}

void GUI_Gauge::action()
{
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
    return (atoi(TextManager::utf16_to_utf8(i->texte).c_str())<atoi(TextManager::utf16_to_utf8(j->texte).c_str()));
}

// class GUI_List : public GUI_Widget
GUI_List::GUI_List(float _pos_x, float _pos_y, float _interligne,	float _intercolonne, int _nb_lignes, TTF_Font *_font, SDL_Color _couleur, SDL_Color* _bgColor)
	: pos_x(_pos_x), pos_y(_pos_y), interligne(_interligne), intercolonne(_intercolonne), nb_lignes(_nb_lignes), font(_font),couleur(_couleur), bgColor(_bgColor)
{
    selectedEntry = NULL;
}

GUI_List::~GUI_List()
{
	for(int i = entries.size()-1; i >=0; --i)
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
    int x = (m_x-pos_x);
    int y = (m_y-pos_y);
    if(x<0 || y <0) return NULL;

	int colonne = x/intercolonne;
	int ligne = y/interligne;

    if(colonne >= 0 && ligne >= 0 && colonne < entries.size()/nb_lignes+1 && ligne < nb_lignes)
    {
        int rang = colonne*nb_lignes+ligne;
        if(rang>=0 && rang < entries.size()) return entries[rang];
    }

	return NULL;
}

ListEntry* GUI_List::setSelectedEntry(int m_x, int m_y)
{
    ListEntry* tmp = selectEntry(m_x, m_y);
    if(tmp!=NULL) selectedEntry=tmp;

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

        float zoom = 1.0;
        float alpha = 0.8;
        if(tmp!=NULL && entries[i] == tmp) zoom = 1.5;
        if(entries[i] == selectedEntry)
        {
            zoom = 1.5;
            alpha = 1.0;
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

bool GUI_List::initFromDirectory(string directoryPath, bool sortAlpha,bool navigationEntries, bool valueWithPath)
{
    entries.erase(entries.begin(), entries.end());

    DIR *dp;
    struct dirent *ep;

    dp = opendir (directoryPath.c_str());
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            if( navigationEntries == true || (string(ep->d_name) != string(".") && string(ep->d_name) != string("..")))
            {
                wstringstream wss;
                wss << ep->d_name;
                stringstream ss;
                if(valueWithPath) ss << directoryPath;
                ss  << ep->d_name;
                addEntry(wss.str(),ss.str());
            }
        }

        closedir (dp);

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
    else
    {
        return false;
    }

}


// class GUI
GUI::GUI(Engine *_engine, string _font, SDL_Color _couleur, SDL_Color* _bgColor, Uint32 _wrapLength)
{
    textInputMode = false;
    this->engine = _engine;
    this->infoBulle = new GUI_InfoBulle(engine->fontPool.getFont(_font),_couleur,_bgColor, _wrapLength);
    focusedWidget = "";
}

GUI::~GUI()
{
    map<string,GUI_Widget*>::iterator p;
    for(p = widgets.begin(); p != widgets.end(); p++)
    {
        delete p->second;
        p->second = NULL;

    }
    delete this->infoBulle;
    this->infoBulle = NULL;
}

void GUI::addWidget(string libelle, GUI_Widget* _widget)
{
    widgets[libelle] = _widget;
}

void GUI::draw()
{
    map<string,GUI_Widget*>::iterator p;
    for(p = widgets.begin(); p != widgets.end(); p++)
    {
        p->second->draw(p->first==this->focusedWidget);

    }

    if(infoBulle != NULL)
    {
        infoBulle->draw();
    }
}

string GUI::rollOver()
{
    map<string,GUI_Widget*>::iterator p;
    for(p = widgets.begin(); p != widgets.end(); p++)
    {
        if(p->second->rollOver(engine->m_x,engine->m_y,engine->display->width,engine->display->height))
        {

            return p->first; // Dès qu'on trouve une infobulle à afficher, on s'arrête
        }
    }
    return "";
}

void GUI::update()
{
    map<string,GUI_Widget*>::iterator p;
    for(p = widgets.begin(); p != widgets.end(); p++)
    {
        p->second->update();
    }
}

bool GUI::clique(string libelle, int m_x, int m_y)
{
    if ( widgets.find(libelle) != widgets.end() )
    {
        // found
        if(widgets[libelle]->clique(m_x,m_y))
        {
            this->setFocusedWidget(libelle);

            widgets[libelle]->action();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
      // not found
      return false;
    }

}

string GUI::clique(int m_x, int m_y)
{
    for (std::map<string,GUI_Widget*>::iterator it=widgets.begin(); it!=widgets.end(); ++it)
    {
        //std::cout << it->first << " => " << it->second << '\n';
        if(it->second->clique(m_x,m_y))
        {
            this->setFocusedWidget(it->first);

            it->second->action();
            return it->first;
        }
    }
    return "";
}


void GUI::startTextInput()
{
    accentMem = '\0';
    textInputMode = true;
    SDL_StartTextInput();
}

void GUI::stopTextInput()
{
    accentMem = '\0';
    textInputMode = false;
    SDL_StopTextInput();
}

GUI_Widget* GUI::getFocusedWidget()
{
    return widgets[focusedWidget];
}

bool GUI::setFocusedWidget(string widget)
{
    focusedWidget = widget;

    // Si le widget qui prend le focus est de type TextInput, on passe en mode saisie de texte
    if(widgets[widget]->widgetType == GUI_WIDGET_TEXTINPUT)
    {
        this->startTextInput();
    }
    else // Sinon on sort du mode saisie de Texte
    {
        this->stopTextInput();
    }
}

bool GUI::textInputKeyDown(const SDL_Event &event)
{
    if(this->textInputMode)
    {
        GUI_TextInput* tmp = dynamic_cast<GUI_TextInput*>(widgets[focusedWidget]);
        //Handle backspace
        if( event.key.keysym.sym == SDLK_BACKSPACE )
        {
            if(tmp->text->texte.length() > 0)
            {
                //lop off character
                tmp->popChar();
                //tmp->text->setTexte(tmp->text->texte.substr(0, tmp->text->texte.length()-1));
            }

        }
        //Handle copy
        else if( event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        {
            SDL_SetClipboardText( TextManager::utf16_to_utf8(tmp->value).c_str() );
            //SDL_SetClipboardText( TextManager::utf16_to_utf8(tmp->text->texte).c_str() );
        }
        //Handle paste
        else if( event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        {
            tmp->setText(TextManager::utf8_to_utf16(SDL_GetClipboardText()));
            //tmp->text->setTexte(TextManager::utf8_to_utf16(SDL_GetClipboardText()));
        }
        else if(event.key.keysym.sym == SDLK_RETURN)
        {
            widgets[focusedWidget]->action();
        }

        return true;
    }
    else
    {
        return false;
    }
}

void GUI::textInputProcessing(const SDL_Event &event)
{
    if(this->textInputMode)
    {
        // Si on n'est pas dans le cas d'un copier/coller, on ajoute le caractère au texte saisi
        if( !( ( event.text.text[ 0 ] == 'c' || event.text.text[ 0 ] == 'C' ) && ( event.text.text[ 0 ] == 'v' || event.text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL ) )
        {
            // TODO : gestion des accents qui se saisissent en 2 fois
            if((unsigned char)(event.text.text[0]) == 0x7e)
            {
                cout << "tilde" << endl;
                accentMem = 0x7e;
            }
            else if((unsigned char)(event.text.text[0]) == 0x5e)
            {
                cout << "circonflexe" << endl;
                accentMem = 0x5e;
            } else if((unsigned char)(event.text.text[0]) == 0xc2 && (unsigned char)(event.text.text[1]) == 0xa8)
            {
                cout << "Tremas" << endl;
                accentMem = 0xa8;
            }


            //cout << hex << +(unsigned char)(event.text.text[0]) << " " << hex << +(unsigned char)(event.text.text[1])<<endl;

            GUI_TextInput* tmp = dynamic_cast<GUI_TextInput*>(widgets[focusedWidget]);
            tmp->appendText(TextManager::utf8_to_utf16(string(event.text.text)));
            //tmp->text->setTexte(tmp->text->texte + TextManager::utf8_to_utf16(string(event.text.text)));
        }
    }
}
