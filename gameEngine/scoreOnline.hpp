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
 
#ifndef _SCORE_ONLINE_HPP
#define _SCORE_ONLINE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "score.hpp"
#include "user.hpp"

using namespace std;

class ScoreTableOnline : public ScoreTable
{
	public:
	vector<ScoreLine> m_score;
	User* user;
	string game;
	string url;
	string host;
	Engine* engine;

	ScoreTableOnline(Engine* _engine, string _game, User* _user, string _url, string _host);
	~ScoreTableOnline();

    virtual void insertScore(std::string name, std::string difficulte,
                    std::string level, int date, int score, int time);

    void insertScore(std::string difficulte,
                    std::string level, int date, int score, int time);

    virtual vector<ScoreLine> get(std::string difficulte,std::string level);
};

#endif
