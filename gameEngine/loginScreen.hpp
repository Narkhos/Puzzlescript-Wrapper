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
 
#include "application.hpp"
#include "gui.hpp"
#include "user.hpp"
#include "textmanager.hpp"

class LoginGUI : public GUI
{
    public:
    LoginGUI(Engine *engine, string _font, SDL_Color _couleur, SDL_Color* _bgColor=NULL, bool _bSubscribe=false);
};

class LoginScreen : public Screen
{
    public:
    LoginGUI* loginGUI;
    string id_screenOk;
    string id_screenCancel;
    string id_screenSubscribe;
    User* user;

    LoginScreen(User* _user, Application* _app, string _id, string _id_screenOk, string _id_screenCancel, string _id_screenSubscribe);
    ~LoginScreen();

    virtual void update();
	virtual void draw();
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_TEXTINPUT(SDL_Event& event);
};

class SubscribeGUI : public GUI
{
    public:
    SubscribeGUI(Engine *engine, string _font, SDL_Color _couleur, SDL_Color* _bgColor=NULL);
};

class SubscribeScreen : public Screen
{
    public:
    SubscribeGUI* subscribeGUI;
    string id_screenOk;
    string id_screenCancel;
    string id_screenLogin;
    User* user;

    SubscribeScreen(User* _user, Application* _app, string _id, string _id_screenOk, string _id_screenCancel, string _id_screenLogin);
    ~SubscribeScreen();

    virtual void update();
	virtual void draw();
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);

    virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_TEXTINPUT(SDL_Event& event);
};
