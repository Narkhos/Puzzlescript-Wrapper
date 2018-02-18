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
 
#include "score.hpp"

bool operator< (ScoreLine const &sl1, ScoreLine const &sl2)
{
    if (sl1.score < sl2.score)
		return true;
	else
		return sl1.time > sl2.time;
}

bool operator== (ScoreLine const &sl1, ScoreLine const &sl2)
{
  return (sl1.score == sl2.score) && (sl1.time == sl2.time);
}

// class ScoreTable
ScoreTable::ScoreTable()
{

}
ScoreTable::~ScoreTable()
{

}
bool ScoreTable::saveXML(string filename)
{
    fstream file;
    file.open(filename.c_str(), fstream::out);
    if(file.is_open())
    {
        int nbEnr = m_score.size();
        file << "<localscore>" << endl;
        for(int i = 0; i < nbEnr; ++i)
        {
            file << "<entree ";
            file << "date = \""<< m_score[i].date<<"\" ";
            file << "name = \""<< m_score[i].name<<"\" ";
            file << "difficulte = \""<< m_score[i].difficulte<<"\" ";
            file << "level = \""<< m_score[i].level<<"\" ";
            file << "score = \""<< m_score[i].score<<"\" ";
            file << "time = \""<< m_score[i].time << "\" />"<<endl;
        }
        file << "</localscore>" << endl;
        file.close();
        return true;
    }
    else
    {
        return false;
    }

}

bool ScoreTable::loadXML(string filename)
{
    IrrXMLReader* xml = createIrrXMLReader(filename.c_str());

    if(!xml)
    {
        cerr << "Lecture du fichier impossible : " << filename << endl;

        return false;
    }
    else
    {
        loadScores(xml);

        // delete the xml parser after usage
        delete xml;
        return true;
    }
}

void ScoreTable::loadScores(IrrXMLReader* xml)
{
    ScoreLine tmpScoreLine;

    m_score.clear();
    while(xml && xml->read())
    {
        switch(xml->getNodeType())
        {
            case EXN_TEXT:
                //on ignore le texte pour l'instant
                //messageText = xml->getNodeData();
            break;
            case EXN_ELEMENT:
                if (!strcmp("entree", xml->getNodeName()))
                {
                    tmpScoreLine.name= xml->getAttributeValue("name");
                    tmpScoreLine.difficulte= xml->getAttributeValue("difficulte");
                    tmpScoreLine.level= xml->getAttributeValue("level");
                    tmpScoreLine.date= atoi(xml->getAttributeValue("date"));
                    tmpScoreLine.score= atoi(xml->getAttributeValue("score"));
                    tmpScoreLine.time= atoi(xml->getAttributeValue("time"));
                    m_score.push_back(tmpScoreLine);
                }

            break;
        }
    }
}

void ScoreTable::insertScore(std::string name, std::string difficulte,
                std::string level, int date, int score, int time)
{
    addScore(name,difficulte,level,date,score,time);
}

void ScoreTable::addScore(std::string name, std::string difficulte,
                std::string level, int date, int score, int time)
{
    ScoreLine tmpScoreLine;
    tmpScoreLine.name = name.substr(0,16);
    tmpScoreLine.difficulte= difficulte;
    tmpScoreLine.level= level;
    tmpScoreLine.date= date;
    tmpScoreLine.score= score;
    tmpScoreLine.time= time;
    m_score.push_back(tmpScoreLine);
    trier();
}

void ScoreTable::trier()
{
    sort(m_score.begin(), m_score.end());
    reverse(m_score.begin(), m_score.end());
}

void ScoreTable::trace(std::string _difficulte, std::string _level)
{
    char *_date;
    int nbEnr = m_score.size();
    for(int i = 0; i<nbEnr;++i)
    {
        if(m_score[i].difficulte.compare(_difficulte) == 0 && m_score[i].level.compare(_level) == 0)
        {
            _date = ctime(&(m_score[i].date));
            cout << m_score[i].name << '\t';
            cout << m_score[i].score << '\t';
            cout << m_score[i].time << '\t';
            cout << _date << endl;
            //delete _date;
            //_date = NULL;
        }
    }
}

 /*
 void ScoreTable::traceConsole(Console &console, std::string _difficulte, std::string _level)
{
    int nbEnr = m_score.size();

    std::ostringstream lvl;

    lvl << _level << " (" << _difficulte << ") : ";
    console.vPushBack("");
    console.vPushBack(lvl.str());
    console.vPushBack("");

    for(int i = 0; i<nbEnr;++i)
    {
        if(m_score[i].difficulte.compare(_difficulte) == 0 && m_score[i].level.compare(_level) == 0)
        {
            std::ostringstream oss;
            oss << (i+1) << ". "<< m_score[i].name << '\t' << m_score[i].score << '\t' << m_score[i].time;
            console.vPushBack(oss.str());
        }
    }
}

void ScoreTable::traceConsoleTimeOnly(Console *console, std::string _difficulte, std::string _level, int rang_deb=1, int nb_score=-1)
{
    int nbEnr = m_score.size();
    int rang = 0;
    int nb_score_trouve = 0;
    std::ostringstream lvl;
    lvl << _level << " (" << _difficulte << ") : ";
    console->vPushBack("");
    console->vPushBack(lvl.str());
    console->vPushBack("");
    std::string espaces = "";


    for(int i = 0; i<nbEnr;++i)
    {
        if(m_score[i].difficulte.compare(_difficulte) == 0 && m_score[i].level.compare(_level) == 0)
        {
            rang++;
            if(rang>=rang_deb)
            {
                espaces = "  ";
                for(int j = m_score[i].name.size(); j < 16;j++)
                {
                        espaces+=" ";
                }

                std::ostringstream oss;
                oss << (rang) << ". "<< m_score[i].name << espaces << '\t' << TimeToHHMMSS(m_score[i].time);
                console->vPushBack(oss.str());
                nb_score_trouve++;
                if(nb_score > 0 && nb_score_trouve>=nb_score) break;
            }
        }
    }

    while(rang < nb_score)
    {
        std::ostringstream oss;
        rang++;
        oss << (rang) << ". -";
        console->vPushBack(oss.str());
    }
}
*/
std::string ScoreTable::TimeToHHMMSS(unsigned int time)
{
    unsigned int heures, minutes, secondes;
    std::ostringstream timeString;
    //time = ((Timer*)m_pTimer)->getCurrentDeviceTime();
    heures = time / 3600000;
    if (heures > 99) heures = 99;
    if (heures < 10)
    time = time % 3600000;
    minutes = time / 60000;
    time = time % 60000;
    secondes = time / 1000;
    time = time % 1000;

    timeString << ((heures < 10)?"0":"")<< heures << ":"
                << ((minutes < 10)?"0":"") << minutes << ":"
                << ((secondes < 10)?"0":"") << secondes<< ":"
                << ((time < 10)?"0":"")<< ((time < 100)?"0":"")<<time;

    return timeString.str();
}

vector<ScoreLine> ScoreTable::get(std::string difficulte,std::string level)
{
    vector<ScoreLine> resultList;
    int nbEnr = m_score.size();
    for(int i = 0; i<nbEnr;++i)
    {
        if(m_score[i].difficulte == difficulte && m_score[i].level==level)
        {
            resultList.push_back(m_score[i]);
        }
    }

    return resultList;
}

/*int main()
{
	ScoreTable scoreTable;

	scoreTable.loadXML("./score.xml");
	scoreTable.trier();
	scoreTable.trace();
	scoreTable.saveXML("./scoresave.txt");

	time_t now;
	time(&now);
	char *_date;
	_date = ctime(&now);
	cout << now << endl;
	cout << "La date d'aujourd'hui est: " << _date;

}*/
