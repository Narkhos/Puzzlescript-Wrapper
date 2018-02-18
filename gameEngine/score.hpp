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
 
#ifndef _SCORE_HPP_
#define _SCORE_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <irrXML.h>
#include <time.h>
#include <algorithm>

using namespace std;
using namespace irr::io;

struct ScoreLine
{
	std::string name;
	unsigned int score;
	std::string difficulte;
	std::string level;
	unsigned int time;
	time_t date;
};

bool operator< (ScoreLine const &sl1, ScoreLine const &sl2);

bool operator== (ScoreLine const &sl1, ScoreLine const &sl2);

class ScoreTable
{
	public:
	vector<ScoreLine> m_score;

	ScoreTable();
	~ScoreTable();
	bool saveXML(string filename);

	bool loadXML(string filename);

    virtual void insertScore(std::string name, std::string difficulte,
                    std::string level, int date, int score, int time);

	void trier();

	void trace(std::string _difficulte, std::string _level);

 /*   void traceConsole(Console &console, std::string _difficulte, std::string _level);

    void traceConsoleTimeOnly(Console *console, std::string _difficulte, std::string _level, int rang_deb=1, int nb_score=-1);
*/
    std::string TimeToHHMMSS(unsigned int time);

    virtual vector<ScoreLine> get(std::string difficulte,std::string level);

    protected:

    void loadScores(IrrXMLReader* xml);
    virtual void addScore(std::string name, std::string difficulte,
                    std::string level, int date, int score, int time);
};

#endif
