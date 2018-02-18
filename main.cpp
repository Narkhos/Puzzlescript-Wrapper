/* ***********************************************************************************
 * Copyright (C) 2018 Xavier Direz - http://www.LaFaceObscureDuPixel.fr              *
 *                                                                                   *
 * This file is part of PuzzlescriptWrapper.                                         *
 *                                                                                   *
 * PuzzlescriptWrapper is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU Lesser General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * PuzzlescriptWrapper is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU Lesser General Public License for more details.                               *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public License          *
 * along with  this program; If not, see <http://www.gnu.org/licenses/>.             *
 *************************************************************************************/

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef __DEBUG__
//#define __DEBUG__
#endif

#ifndef __PS_WRAPPER_VERSION__
#define __PS_WRAPPER_VERSION__ "v0.1.0"
#endif

#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include "chemins.h"
#include "configSDL.hpp"
#include "display.hpp"
#include "SDL2/SDL_net.h"
#include "engine.hpp"
#include "puzzlescriptWrapper.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    string game = "game";

    if(argc>1)
    {
        string arg = string(argv[1]);
        if(arg == "/v" || arg == "-v" || arg == "--version")
        {
            cout << __PS_WRAPPER_VERSION__ << endl;
            return 0;
        }
        else
        {
            game = arg;
        }
    }

    Engine engine("Puzzlescript", 1024, 768, false, false);
    puzzlescriptWrapper(engine, game);

	return 0;
}

