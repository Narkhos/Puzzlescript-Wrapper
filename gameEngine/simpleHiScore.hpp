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
 
#ifndef __SIMPLEHISCORE_HPP__
#define __SIMPLEHISCORE_HPP__

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
/**
    Permet de gérer la sauvegarde et le chargement du meilleur score d'un jeu ou d'un niveau d'un jeu.
*/
class SimpleHiScore
{
    public:
    string bestScore;   ///< Meilleur score actuel
    string scoreFile;   ///< Chemin du fichier ou est enregistré (ou doit être enregistré) le score

    /**
        Constructeur par défaut
    */
    SimpleHiScore();

    /**
        @param _scoreFile Chemin du fichier contenant, ou devant contenir le score
    */
    SimpleHiScore(string _scoreFile);

    ~SimpleHiScore();

    /**
        Doit être implémenter par la classe fille de manière à pouvoir comparer l'ancien score avec le nouveau
        @param currentScore Score à comparer avec le hi-score actuel
        @return renvoi vrai si le nouveau score est meilleur
    */
    virtual bool isBetter(string currentScore)=0;

    /**
        Tente de sauvegarder le nouveau score.
        @param currentScore Le nouveau score à enregistrer
        @return retourne true si le score a été enregistré. false s'il était moins bien que celui existant ou qu'il n'a pas pu être sauvegardé
    */
    bool saveNewHiScore(string currentScore);

    /**
        Charge le hi-score depuis le fichier
        @return renvoi true si le score a pu être récupéré depuis le fichier
    */
    bool loadHiScore(string _scoreFile);
};

#endif
