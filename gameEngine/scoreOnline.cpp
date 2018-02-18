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
 
#include "scoreOnline.hpp"

ScoreTableOnline::ScoreTableOnline(Engine* _engine, string _game, User* _user, string _url, string _host)
    :engine(_engine), game(_game), user(_user), url(_url), host(_host)
{
}

ScoreTableOnline::~ScoreTableOnline()
{
}

void ScoreTableOnline::insertScore(std::string name, std::string difficulte,
                std::string level, int date, int score, int time)
{
    addScore(name,difficulte,level,date,score,time);

    // Envoi du score au serveur
    NetworkClient client(80,10000, this->url, engine);
    client.init();

    stringstream ss;
    ss << "GET /scores/addScore.php?game="<< this->game;
    ss << "&user=" << name;
    ss << "&difficulte=" << difficulte;
    ss << "&level=" << level;
    ss << "&date=" << date;
    ss << "&score=" << score;
    ss << "&time=" << time;
    ss << " HTTP/1.0\r\n";
    ss << "Host: " << this->host<< "\r\n";
    ss << "Connection: Close\r\n\r\n";
    client.sendMessage(ss.str());

    client.getMessage(5000);

}

void ScoreTableOnline::insertScore(std::string difficulte,
                std::string level, int date, int score, int time)
{
    insertScore(this->user->login,difficulte, level, date, score, time);
}

vector<ScoreLine> ScoreTableOnline::get(std::string difficulte,std::string level)
{
    NetworkClient client(80,10000, this->url, engine);
    client.init();

    stringstream ss;
    ss << "GET /scores/score.php?game="<< this->game;
    ss << "&difficulte=" << difficulte;
    ss << "&level=" << level;
    ss << " HTTP/1.0\r\n";
    ss << "Host: " << this->host<< "\r\n";
    ss << "Connection: Close\r\n\r\n";
    client.sendMessage(ss.str());


    ofstream f("./data/scoreOnline.xml");

    f << client.getMessage(5000);
    f.close();


    this->loadXML("./data/scoreOnline.xml");

    return this->m_score;
}
