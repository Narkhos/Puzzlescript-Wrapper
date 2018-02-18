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
 
#ifndef __CONFIGSDL_HPP__
#define __CONFIGSDL_HPP__

#include <irrXML.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstring>
using namespace irr; // irrXML is located in the namespace irr::io
//using namespace video;
using namespace io;
using namespace std;

/*
<?xml version="1.0"?>
<config>
<!-- Parametrage d'application SDL -->
<fullscreen value="OFF" />
<antialiazing value="OFF" />
<windowSize width="1024" height="768" />
<music value="ON" />
<sfx value="ON" />
<language value="fr" />
</config>
*/
class ConfigSDL
{
    private:

        bool bCharge;
        std::string cCheminFichier;
        IrrXMLReader* xml;

    public:

        bool antialiazing;
        bool fullscreen;
		int width;
        int height;
        bool music;
        bool sfx;

		std::string language;

        ConfigSDL(std::string _cCheminFichier = "")
            :cCheminFichier(_cCheminFichier), bCharge(false), xml(NULL)
        {
            bCharger(_cCheminFichier);
        }

        ~ConfigSDL()
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
            myfile << "<config>" << endl;
            myfile << " <!-- Application Params -->" << endl;
            myfile << "<antialiazing value=\"" << (antialiazing?"ON":"OFF") << "\" />" << endl;
            myfile << "<fullscreen value=\"" << (fullscreen?"ON":"OFF") << "\" />" << endl;
            myfile << "<music value=\"" << (music?"ON":"OFF") << "\" />" << endl;
            myfile << "<sfx value=\"" << (sfx?"ON":"OFF") << "\" />" << endl;
            myfile << "<windowSize width=\"" << width << "\" height=\"" << height << "\" />" << endl;
			myfile << "<language value=\"" << language << "\" />" << endl;
            myfile << "</config>" << endl;
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
                    this->vSetDefault();
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
                                        if (!strcmp("antialiazing", xml->getNodeName()))
                                                antialiazing = (!strcmp(xml->getAttributeValue("value"),"ON")?true:false);
                                        else
                                        if (!strcmp("fullscreen", xml->getNodeName()))
                                                fullscreen = (!strcmp(xml->getAttributeValue("value"),"ON")?true:false);
                                        else
                                        if (!strcmp("music", xml->getNodeName()))
                                                music = (!strcmp(xml->getAttributeValue("value"),"ON")?true:false);
                                        else
										if (!strcmp("sfx", xml->getNodeName()))
                                                sfx = (!strcmp(xml->getAttributeValue("value"),"ON")?true:false);
                                        else
                                        if (!strcmp("language", xml->getNodeName()))
                                                language = xml->getAttributeValue("value");
                                        else
                                        if (!strcmp("windowSize", xml->getNodeName()))
                                        {
                                                width = atoi(xml->getAttributeValue("width"));
                                                height = atoi(xml->getAttributeValue("height"));
                                        }
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

        void vSetDefault()
        {
            bCharge = false;

            antialiazing = false;
            fullscreen = false;
            width = 640;
            height = 480;
            music = true;
            sfx = true;
			language = "en";
        }

        void vAfficher()
        {
            //cout << messageText << endl;
            cout << "antialiazing : " << (antialiazing?"ON":"OFF") <<endl;
//            cout << "Fullscreen : " << (fullscreen?"ON":"OFF") <<endl;
            cout << "windowSize : " << width << "x" << height << endl;
            cout << "Music : " << (music?"ON":"OFF") <<endl;
            cout << "Sfx : " << (sfx?"ON":"OFF") <<endl;
			cout << "Language : " << language << endl;
            return;
        }
};

#endif
