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
 
#include "loginScreen.hpp"

//class LoginGUI : public GUI

LoginGUI::LoginGUI(Engine *engine, string _font, SDL_Color _couleur, SDL_Color* _bgColor, bool _bSubscribe)
    :GUI(engine,_font,_couleur,_bgColor,200)
{

    int _x1=0;
    int _y1=0;

    int _x2=0;
    int _y2=50;

    this->addWidget(string("btLogin"),new GUI_Button("themes/default/textures/buttons/login.png", engine->display->width-16,0,16,16,1, this,L"Login"));
    this->addWidget(string("btCancel"),new GUI_Button("themes/default/textures/buttons/cancel.png", engine->display->width-16,16,16,16,1, this,L"Cancel"));
    this->addWidget(string("btOffline"),new GUI_Button("themes/default/textures/buttons/offline.png", engine->display->width-16,32,16,16,1, this,L"Play Offline"));

    if(_bSubscribe)
    {
        this->addWidget(string("btSubscribe"),new GUI_Button("themes/default/textures/buttons/subscribe.png", engine->display->width-16,48,16,16,1, this,L"Subscribe"));
    }

    this->addWidget(string("txtLogin"),new GUI_TextInput(engine->fontPool.getFont(_font), _couleur, _x1, _y1, this,
                        engine->texturePool.getTexture("bg_textfield.png"), engine->texturePool.getTexture("text_cursor.png"), 16, false));
    this->addWidget(string("txtPass"),new GUI_TextInput(engine->fontPool.getFont(_font), _couleur, _x2, _y2, this,
                        engine->texturePool.getTexture("bg_textfield.png"), engine->texturePool.getTexture("text_cursor.png"), 16, true));
}


// class LoginScreen : public Screen

LoginScreen::LoginScreen(User* _user, Application* _app, string _id, string _id_screenOk, string _id_screenCancel, string _id_screenSubscribe)
    :Screen(_app,_id), user(_user), id_screenOk(_id_screenOk), id_screenCancel(_id_screenCancel), id_screenSubscribe(_id_screenSubscribe)
{
    SDL_Color textColor = {255 , 255 , 255, 255};	///< couleur utilisée pour l'affichage du texte
	SDL_Color bulleColor = {0 , 0 , 200, 100};	    ///< couleur utilisée pour l'affichage du fond des infoBulle

    loginGUI = new LoginGUI(app->engine, "OxygenMono-Regular.ttf",textColor,&bulleColor, _id_screenSubscribe!="");
}

LoginScreen::~LoginScreen()
{
    delete loginGUI;
}

void LoginScreen::update()
{
    loginGUI->update();
}

void LoginScreen::event_TEXTINPUT(SDL_Event& event)
{
    loginGUI->textInputProcessing(event);
}

void LoginScreen::event_KEYDOWN(SDL_Event& event)
{
    // On demande à la gui de gérer le champ de saisie si nécessaire.
    if(!loginGUI->textInputKeyDown(event))
    {
        // Si on n'est pas dans un champ de saisie, on gère les inputs clavier normalement ici
    }
}

void LoginScreen::event_MOUSEBUTTONDOWN(SDL_Event& event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        string clic = loginGUI->clique(app->engine->m_x,app->engine->m_y);
        if(clic=="btLogin")
        {
            // Créer l'utilisateur et Contrôler le couple login/mdp
            user->login = TextManager::utf16_to_utf8(dynamic_cast<GUI_TextInput*>(loginGUI->widgets["txtLogin"])->value);
            user->password = TextManager::utf16_to_utf8(dynamic_cast<GUI_TextInput*>(loginGUI->widgets["txtPass"])->value);

            // Si c'est bon, on peut lancer l'écran suivant (id_screenOk)
            string retour = user->check();
            if(retour=="OK")
            {
                cout << "Authentification OK" << endl;
                app->setScreen(id_screenOk);
            }
            else
            {
                cout << "Echec de l'authentification : " << retour << endl;
            }

            // Sinon, on demande a l'utilisateur de recommencer
        } else if(clic=="btCancel")
        {
            // Retourner à l'écran précédent
            app->setScreen(id_screenCancel);

        } else if (clic=="btSubscribe")
        {
            app->setScreen(id_screenSubscribe);
        } else if(clic=="btOffline")
        {
            cout << "Jeu offline" << endl;
            app->setScreen(id_screenOk);
        }
    }
}

void LoginScreen::draw()
{
    // Affichage de la GUI
    loginGUI->rollOver();
    loginGUI->draw();
}

void LoginScreen::onSet(string id_orig)
{
    this->loginGUI->setFocusedWidget("txtLogin");
}

void LoginScreen::onQuit(string id_dest)
{
}

//class SubscribeGUI : public GUI

SubscribeGUI::SubscribeGUI(Engine *engine, string _font, SDL_Color _couleur, SDL_Color* _bgColor)
    :GUI(engine,_font,_couleur,_bgColor,200)
{

    int _x1=0;
    int _y1=0;

    int _x2=0;
    int _y2=50;

    int _x3 = 0;
    int _y3 = 100;

    this->addWidget(string("btLogin"),new GUI_Button("themes/default/textures/buttons/login.png", engine->display->width-16,0,16,16,1, this,L"Login"));
    this->addWidget(string("btCancel"),new GUI_Button("themes/default/textures/buttons/cancel.png", engine->display->width-16,16,16,16,1, this,L"Cancel"));
    this->addWidget(string("btOffline"),new GUI_Button("themes/default/textures/buttons/offline.png", engine->display->width-16,32,16,16,1, this,L"Play Offline"));

    this->addWidget(string("txtMail"),new GUI_TextInput(engine->fontPool.getFont(_font), _couleur, _x1, _y1, this,
                        engine->texturePool.getTexture("bg_textfield.png"), engine->texturePool.getTexture("text_cursor.png"), 16, false));
    this->addWidget(string("txtLogin"),new GUI_TextInput(engine->fontPool.getFont(_font), _couleur, _x2, _y2, this,
                        engine->texturePool.getTexture("bg_textfield.png"), engine->texturePool.getTexture("text_cursor.png"), 16, false));
    this->addWidget(string("txtPass"),new GUI_TextInput(engine->fontPool.getFont(_font), _couleur, _x3, _y3, this,
                        engine->texturePool.getTexture("bg_textfield.png"), engine->texturePool.getTexture("text_cursor.png"), 16, true));

}

// class SubscribeScreen : public Screen

SubscribeScreen::SubscribeScreen(User* _user, Application* _app, string _id, string _id_screenOk, string _id_screenCancel, string _id_screenLogin)
    :Screen(_app,_id), user(_user), id_screenOk(_id_screenOk), id_screenCancel(_id_screenCancel), id_screenLogin(_id_screenLogin)
{
    SDL_Color textColor = {255 , 255 , 255, 255};	///< couleur utilisée pour l'affichage du texte
	SDL_Color bulleColor = {0 , 0 , 200, 100};	    ///< couleur utilisée pour l'affichage du fond des infoBulle

    subscribeGUI = new SubscribeGUI(app->engine, "OxygenMono-Regular.ttf",textColor,&bulleColor);
}

SubscribeScreen::~SubscribeScreen()
{
    delete subscribeGUI;
}

void SubscribeScreen::update()
{
    subscribeGUI->update();
}


void SubscribeScreen::event_TEXTINPUT(SDL_Event& event)
{
    subscribeGUI->textInputProcessing(event);
}

void SubscribeScreen::event_KEYDOWN(SDL_Event& event)
{
    // On demande à la gui de gérer le champ de saisie si nécessaire.
    if(!subscribeGUI->textInputKeyDown(event))
    {
        // Si on n'est pas dans un champ de saisie, on gère les inputs clavier normalement ici
    }

}

void SubscribeScreen::event_MOUSEBUTTONDOWN(SDL_Event& event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        string clic = subscribeGUI->clique(app->engine->m_x,app->engine->m_y);
        if(clic=="btLogin")
        {
            // Créer l'utilisateur et Contrôler le couple login/mdp
            user->email = TextManager::utf16_to_utf8(dynamic_cast<GUI_TextInput*>(subscribeGUI->widgets["txtMail"])->value);
            user->login = TextManager::utf16_to_utf8(dynamic_cast<GUI_TextInput*>(subscribeGUI->widgets["txtLogin"])->value);
            user->password = TextManager::utf16_to_utf8(dynamic_cast<GUI_TextInput*>(subscribeGUI->widgets["txtPass"])->value);

            // Si c'est bon, on peut lancer l'écran suivant (id_screenOk)
            string retour = user->inscrire();

            if(retour=="OK")
            {
                cout << "Inscription OK" << endl;
                app->setScreen(id_screenOk);
            }
            else
            {
                cout << "Echec de l'authentification : " << retour << endl;
            }

            // Sinon, on demande a l'utilisateur de recommencer
        } else if(clic=="btCancel")
        {
            // Retourner à l'écran précédent
            app->setScreen(id_screenCancel);

        } else if(clic=="btOffline")
        {
            cout << "Jeu offline" << endl;
            app->setScreen(id_screenOk);
        }
    }
}

void SubscribeScreen::draw()
{
    // Affichage de la GUI
    subscribeGUI->rollOver();
    subscribeGUI->draw();
}

void SubscribeScreen::onSet(string id_orig)
{
    this->subscribeGUI->setFocusedWidget("txtMail");
}

void SubscribeScreen::onQuit(string id_dest)
{
}

