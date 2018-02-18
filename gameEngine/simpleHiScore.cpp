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
 
#include "simpleHiScore.hpp"


//class SimpleHiScore

SimpleHiScore::SimpleHiScore()
{
}

SimpleHiScore::SimpleHiScore(string _scoreFile)
{
    loadHiScore(_scoreFile);
}

SimpleHiScore::~SimpleHiScore()
{
}

bool SimpleHiScore::saveNewHiScore(string currentScore)
{
    if(bestScore=="" || isBetter(currentScore))
    {
        bestScore = currentScore;
        // Enregistrement du nouveau Hi-score
        ofstream ofs;
        ofs.open(scoreFile.c_str(), std::ifstream::out);
        if(ofs.is_open())
        {
            ofs<<bestScore;
            ofs.close();
            return true;
        }
        else
        {
            if(scoreFile=="")
            {
                cout << "Impossible de sauvegarder le score. Aucun fichier n'a été spécifié" << endl;
            }
            else
            {
                cout << "Impossible de sauvegarder le score. Fichier introuvable : " << scoreFile << endl;
            }
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool SimpleHiScore::loadHiScore(string _scoreFile)
{
    scoreFile=_scoreFile;

    ifstream ifs;
    ifs.open(scoreFile.c_str(), std::ifstream::in);
    // Si le fichier existe, On a déjà fait un hi-score sur le jeu/niveau
    if(ifs.is_open())
    {
        // Chargement du score depuis le fichier
        getline(ifs, bestScore);
        ifs.close();
        return true;
    }
    else
    {
        // Le fichier de score n'existe pas encore
        bestScore = "";
        return false;
    }
}
