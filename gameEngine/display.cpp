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

#include <stdio.h>
#include <stdlib.h>
#include "display.hpp"

Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;

	//Get the requested pixel
	return pixels[ ( y * surface->w ) + x ];
}

void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;

	//Set the pixel
	pixels[ ( y * surface->w ) + x ] = pixel;
}

void drawImage(GLuint tex, float x, float y, float w, float h, float alpha, GLfloat luminosite , GLfloat texCoord[8])
{

//#ifdef GLES_VERSION

	GLfloat vtx1[] =
	{
		x, y, 0.0,
		x+w, y, 0.0,
		x+w, y+h, 0.0,
		x, y+h, 0.0
	};

	GLfloat texCoord1[] =
	{
		0.0,0.0,
		1.0,0.0,
		1.0,1.0,
		0.0,1.0
	};

#ifdef NO_TEXTURES
	// Gestion des couleurs unies dans le cas ou les textures ne fonctionnent pas
	float c_r,c_v,c_b;
	if(tex==tex_r) { c_r=1;c_v=0;c_b=0; }
	if(tex==tex_v) { c_r=0;c_v=1;c_b=0; }
	if(tex==tex_b) { c_r=0;c_v=0;c_b=1; }
	if(tex==tex_j) { c_r=1;c_v=1;c_b=0; }
	if(tex==tex_o) { c_r=1;c_v=1;c_b=1; }

	GLfloat col1[] =
	{
		c_r, c_v, c_b, alpha,
		c_r, c_v, c_b, alpha,
		c_r, c_v, c_b, alpha,
		c_r, c_v, c_b, alpha
	};
	//

#else
	GLfloat col1[] =
	{
		luminosite,luminosite,luminosite, alpha,
		luminosite,luminosite,luminosite, alpha,
		luminosite,luminosite,luminosite, alpha,
		luminosite,luminosite,luminosite, alpha
	};
#endif


	glBindTexture( GL_TEXTURE_2D, tex );

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vtx1);
	if(texCoord!=NULL)
    {
        glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
    }
    else
    {
        glTexCoordPointer(2, GL_FLOAT, 0, texCoord1);
    }


    glColorPointer(4, GL_FLOAT, 0, col1);


	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, const std::wstring& texte, int &w, int &h)
{
	SDL_Surface *s = TTF_RenderUNICODE_Blended(const_cast<TTF_Font*>(font), TextManager::wstringToUint16(texte), color);
    if(!s)
    {
        cout << "Erreur de génération de la texture de texte : " << TTF_GetError() << endl;
        cout << endl;
    }

	GLuint tid = 0;
	GLint ncolors;
	GLenum texture_format;
	/* Convert SDL_Surface to OpenGL Texture */
	ncolors = s->format->BytesPerPixel;
	if (ncolors == 4) {
		//alpha channel
		if (s->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	} else {
		if (ncolors == 3) {
			//no alpha channel
			if (s->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} else {
			return 0;
		}
	}

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, ncolors, s->w, s->h, 0,
			texture_format, GL_UNSIGNED_BYTE, s->pixels);

	// On retourne les dimensions de la texture créée
	w = s->w;
	h = s->h;

    delete s; s = NULL;
	return tid;
}

GLuint texteToTexture(const TTF_Font *font, SDL_Color color, SDL_Color bgColor, const std::wstring& texte, int &w, int &h,Uint32 wrapLength)
{
	SDL_Surface *t;
    if(wrapLength > 0)
	{
		t = TTF_RenderUNICODE_Blended_Wrapped(const_cast<TTF_Font*>(font), TextManager::wstringToUint16(texte), color,wrapLength);
	}
	else
	{
		t = TTF_RenderUNICODE_Blended(const_cast<TTF_Font*>(font), TextManager::wstringToUint16(texte), color);
	}

	SDL_Surface *s = SDL_CreateRGBSurface(0, t->w + 4, t->h + 4, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_BLEND);


    SDL_FillRect(s, NULL, SDL_MapRGBA(s->format , bgColor.r , bgColor.g , bgColor.b, bgColor.a ) );


    SDL_Rect cible;
    cible.x = 2; cible.y = 2; cible.w = t->w; cible.h = t->h;

    SDL_BlitSurface(t, NULL, s, &cible);


	GLuint tid = 0;
	GLint ncolors;
	GLenum texture_format;
	/* Convert SDL_Surface to OpenGL Texture */
	ncolors = s->format->BytesPerPixel;
	if (ncolors == 4) {
		//alpha channel
		if (s->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	} else {
		if (ncolors == 3) {
			//no alpha channel
			if (s->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} else {
			return 0;
		}
	}

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, ncolors, s->w, s->h, 0,
			texture_format, GL_UNSIGNED_BYTE, s->pixels);

	// On retourne les dimensions de la texture créée
	w = s->w;
	h = s->h;

    delete s; s = NULL;
	return tid;
}

GLuint createRedTexture()
{
	int imageWidth = 128;
	int imageHeight = 128;

	GLuint texId;
	glGenTextures( 1, &texId );
	glBindTexture( GL_TEXTURE_2D, texId );

	GLubyte* image_data_temp = new GLubyte[imageWidth * imageHeight * 3];
	GLubyte* dst_image = image_data_temp;
	for (unsigned int i = 0; i < imageWidth * imageHeight; i++)
	{
	    // this will fill the texture with red (0xFF0000)
	    // ... unless I've flipped the byte order, in which case it will be blue
	    dst_image[0] = 0xFF;
	    dst_image[1] = 0x00;
	    dst_image[2] = 0x00;
	    dst_image += 3;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageWidth,
		    imageHeight, 0, GL_RGB,
		    GL_UNSIGNED_BYTE, image_data_temp );

	// if you have a "new", you must have a "delete" too!
	delete [] image_data_temp;

	return texId;
}

GLuint SurfaceToTexture2(SDL_Surface *surface)
{
    //return createRedTexture();
	GLuint tid;
	GLenum texture_format;
	GLint ncolors;
	SDL_Surface* s = surface;

	/* Convert SDL_Surface to OpenGL Texture */
	ncolors = s->format->BytesPerPixel;
	if (ncolors == 4) {
		//alpha channel
		if (s->format->Rmask == 0x000000ff)
		{
			texture_format = GL_RGBA;
        }
		else
		{
			texture_format = GL_BGRA;
        }
	} else {
		if (ncolors == 3) {
			//no alpha channel
			if (s->format->Rmask == 0x0000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} else {
			return 0;
		}
	}

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, ncolors, s->w, s->h, 0,
			texture_format, GL_UNSIGNED_BYTE, s->pixels);

	return tid;
}

// class Image


Image::Image(string path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());

    if(!surface)
    {
        cout << "IMG_Load : " << IMG_GetError() << endl;
        tex = -1;
    }
    else
    {

        tex = SurfaceToTexture2(surface);
        SDL_FreeSurface(surface); surface = NULL;

        glBindTexture(GL_TEXTURE_2D,(GLuint)(this->tex));

        // TODO : permettre le paramétrage par l'utilisateur de ces paramètres
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    }
}

Image::~Image()
{
}

void Image::draw(float x, float y, float w, float h, float alpha, GLfloat luminosite, GLfloat texCoord[8])
{
    drawImage(tex,x,y,w,h,alpha, luminosite, texCoord);
}

// Class Display

Display::Display(string title, unsigned int _width, unsigned int _height):	width(_width),
        height(_height),ratio((float) _width / (float) _height),
        flags(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)
    {
        current_width = width;
        current_height = height;

        init(title);
    	/*//flags = flags | SDL_WINDOW_FULLSCREEN;
        init_video_mode();
        this->initOpenGL(screen, _width, _height);
        screenSurface = SDL_GetWindowSurface(screen);*/
    }

Display::Display(string title, ConfigSDL& cfg, unsigned int _native_width, unsigned int _native_height):flags(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)
{
    current_width = cfg.width;
    current_height = cfg.height;

    if(_native_width>0 && _native_height>0)
    {
        width = _native_width;
        height = _native_height;
    }
    else
    {
        width = cfg.width;
        height = cfg.height;
    }
    this->ratio = (float) width / (float) height;
    if(cfg.fullscreen) flags = flags | SDL_WINDOW_FULLSCREEN;

    init(title);
}

void Display::init(string title)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    this->screen = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, current_width, current_height, flags);
    SDL_GLContext glcontext = SDL_GL_CreateContext(screen);
    cout << "OpenGL Version " << glGetString(GL_VERSION) << "\n";


    glClearColor(0,0,0,1);
    glViewport( 0, 0, current_width, current_height );
    glOrtho(0, width, height, 0, -1, 1);

    screenSurface = SDL_GetWindowSurface(screen);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialisation de glew pour la gestion des shaders
    GLenum err = glewInit();
    if (err != GLEW_OK)
      exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
      exit(1); // or handle the error in a nicer way

    int nbMappingsAdded = SDL_GameControllerAddMappingsFromFile("./data/gamecontrollerdb.txt");
	if(nbMappingsAdded==-1)
	{
		cout << SDL_GetError() << endl;
	}
	else
	{
		cout << "Nombre de mappings ajoutes : " << nbMappingsAdded << endl;
	}
}

 Display::~Display()
 {
    SDL_Quit();
 }
