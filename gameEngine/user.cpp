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
 
#include "user.hpp"

// class User


User::User(Engine* _engine, string _login, string _password, string _url)
    :engine(_engine),login(_login), password(_password), url(_url), host(url)
{
}

User::User(Engine* _engine, string _login, string _password, string _url, string _host)
    :engine(_engine),login(_login), password(_password), url(_url), host(_host)
{
}

User::User(Engine* _engine, string _url)
    :engine(_engine),login(""), password(""), url(_url), host(url)
{
}

User::User(Engine* _engine, string _url, string _host)
    :engine(_engine),login(""), password(""), url(_url), host(_host)
{
}

User::~User()
{
}

string User::check()
{
    stringstream ss;
    NetworkClient client(80,10000, string(this->url), this->engine);
    client.init();

    ss << "GET /scores/checkuser.php?";
    ss << "login=" << this->login;
    ss << "&pass=" << this->password;
    ss << " HTTP/1.0\r\n";
    ss << "Host: " << this->host << "\r\n";
    ss << "Connection: Close\r\n\r\n";

    cout << ss.str() << endl;
    client.sendMessage(ss.str());

    return client.getMessage(5000);
}

string User::inscrire(string _email)
{
    if(_email!="")
    {
        this->email = _email;
    }

    stringstream ss;
    NetworkClient client(80,10000, string(this->url), this->engine);
    client.init();

    ss << "GET /scores/adduser.php?";
    ss << "login=" << this->login;
    ss << "&pass=" << this->password;
    ss << "&mail=" << this->email;
    ss << " HTTP/1.0\r\n";
    ss << "Host: " << this->host << "\r\n";
    ss << "Connection: Close\r\n\r\n";

    client.sendMessage(ss.str());
    return client.getMessage(5000);
}

string User::inscrire(string _email, string _login,string _password)
{
    this->login=_login;
    this->password = _password;
    return inscrire(_email);
}
