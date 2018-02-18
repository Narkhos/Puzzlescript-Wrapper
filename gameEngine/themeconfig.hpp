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
 
#ifndef __THEMECONFIG_HPP__
#define __THEMECONFIG_HPP__

#include <irrXML.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace irr; // irrXML is located in the namespace irr::io
using namespace io;
using namespace std;

/*
<?xml version="1.0"?>
<theme>
<!-- Parametrage Theme graphique -->
<reflet value="0.5" /> <!-- intensité du reflet entre 0.0 et 1.0 -->
<marge value="56" /> <!-- en pixels sur la texture -->
<epaisseur value="0.5" /> <!-- 1.0 = largeur d'une case -->
</theme>
*/

class ThemeConfig
{
    private:

        bool bCharge;
        std::string cCheminFichier;
        IrrXMLReader* xml;

    public:

        float reflet;
		int marge;
		float epaisseur;
		bool cadre;

        ThemeConfig(std::string _cCheminFichier = "")
            :cCheminFichier(_cCheminFichier), bCharge(false), xml(NULL)
        {
            vNettoyer();
        }

        ~ThemeConfig()
        {}

        bool bSauver(std::string _cCheminFichier = "")
        {
            ofstream myfile;
            if(_cCheminFichier == "")
            {
                _cCheminFichier = cCheminFichier;
            }

            if(_cCheminFichier == "")
            {
                return false;
            }

            myfile.open(_cCheminFichier.c_str());

            if(!myfile.is_open())
            {
                return false;
            }

            myfile << "<?xml version=\"1.0\"?>" << endl;
            myfile << "<theme>" << endl;
            myfile << " <!-- Parametrage Theme graphique -->" << endl;
            myfile << "<reflet value=\"" << reflet << "\" />" << endl;
            myfile << "<marge value=\"" << marge << "\" />" << endl;
			myfile << "<epaisseur value=\"" << epaisseur << "\" />" << endl;
			myfile << "<cadre value=\"" << (cadre?"ON":"OFF") << "\" />" << endl;
            myfile << "</theme>" << endl;
            myfile.close();

            return true;
        }
        bool bCharger(std::string _cCheminFichier = "")
        {
            bool bRetour = false;
            if(_cCheminFichier != "")
            {
                cCheminFichier = _cCheminFichier;
            }

            if(cCheminFichier != "")
            {
                if (bCharge == true)
                {
                    vNettoyer();
                }

                xml = createIrrXMLReader(cCheminFichier.c_str());

                while(xml && xml->read())
                {
                        switch(xml->getNodeType())
                        {
                            case EXN_TEXT:
                                /*if(strcmp("messageText", xml->getNodeName()))
                                {
                                    messageText = xml->getNodeData();
                                }*/
                                break;
                            case EXN_ELEMENT:
                                {
                                        if (!strcmp("reflet", xml->getNodeName()))
                                                reflet = atof(xml->getAttributeValue("value"));
                                        else
                                        if (!strcmp("marge", xml->getNodeName()))
                                                marge = atoi(xml->getAttributeValue("value"));
                                        else
                                         if (!strcmp("epaisseur", xml->getNodeName()))
                                                epaisseur = atof(xml->getAttributeValue("value"));
										else
                                         if (!strcmp("cadre", xml->getNodeName()))
                                                cadre = (!strcmp(xml->getAttributeValue("value"),"ON")?true:false);
                                }
                                break;
                        }
                }

                // delete the xml parser after usage
                delete xml;
                xml = NULL;
                bCharge = true;
            }

            return bRetour;
        }

        void vNettoyer()
        {
            bCharge = false;

            reflet = 0.25;
			marge = 0;
			epaisseur = 1.0;
			cadre = true;
        }

        void vAfficher()
        {
            //cout << messageText << endl;
            /*cout << "antialiazing : " << (antialiazing?"ON":"OFF") <<endl;
            cout << "Fullscreen : " << (fullscreen?"ON":"OFF") <<endl;
            cout << "windowSize : " << width << "x" << height << endl;
            cout << "Music : " << (music?"ON":"OFF") <<endl;
            cout << "Sfx : " << (sfx?"ON":"OFF") <<endl;
			cout << "Language : " << language << endl;*/
            return;
        }
};

#endif
