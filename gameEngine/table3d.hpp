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
 
#ifndef __TABLE3D_HPP__
#define __TABLE3D_HPP__

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "table3dUtility.hpp"


using namespace std;

template <typename T>
class Table3d
{
	public:
	T**** data;
	int width;
	int height;
	int depth;

	// Constructeur par copie
	Table3d(const Table3d& t)
	{
		//cout << "Constructeur par copie de Table3d" << endl;
		this->width = t.width;
		this->height = t.height;
		this->depth = t.depth;

		data = new T* **[width]();
		for (int i = 0; i < width; i++)
		{
			data[i] = new T* *[height]();
			for (int j = 0; j < height; j++)
			{

				data[i][j] = new T* [depth]();
				for (int k = 0; k < depth; k++)
				{
					data[i][j][k] = new T(*(t.data[i][j][k]));
				}
			}
		}
	}

	Table3d()
	{
		data = NULL;
		width = 0;
		height = 0;
		depth = 0;
	}

	Table3d(int _width, int _height, int _depth, bool remplir=true)
        : width(0),height(0),depth(0)
	{
	    data = NULL;
		init(_width, _height, _depth, remplir);
	}

	~Table3d()
	{
		freeData();
	}

	void freeData()
	{
	    if(data == NULL) return;

		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int k = 0; k < depth; k++)
				{
					delete data[i][j][k];
					data[i][j][k] = NULL;
				}

				delete[] data[i][j];
			}

			delete[] data[i];
		}
		delete[] data;
		data = NULL;
	}

	void init(int _width, int _height, int _depth, bool remplir=true)
	{

		freeData();

		this->width=_width;
		this->height=_height;
		this->depth=_depth;

		data = new T* **[width]();
		for (int i = 0; i < width; i++)
		{
			data[i] = new T* *[height]();
			for (int j = 0; j < height; j++)
			{

				data[i][j] = new T* [depth]();
				for (int k = 0; k < depth; k++)
				{
				    if(remplir)
                    {
                        data[i][j][k] = new T();
                    }
                    else
                    {
                        data[i][j][k] = NULL;
                    }

				}
			}
		}
	}


	virtual void trace()
	{
		for (int k = 0; k < depth; k++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{
					if(data[i][j][k])
					{
						data[i][j][k]->trace();
					}
					else
					{
						cout << "NULL" << endl;
					}
					//cout << " ";
				}
				cout << endl;
			}
			cout << endl;
		}
	}

	virtual void copyToClipBoard()
	{
	    stringstream ss;

	    for (int k = 0; k < depth; k++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{
					if(data[i][j][k])
					{
						data[i][j][k]->copyToClipBoard(ss);
					}
					else
					{
						ss << "<NULL>";
					}

				}
				ss << '\n';
			}
		}
        SDL_SetClipboardText(ss.str().c_str());
	}

    /**
        Vérifie que la position indiquée est dans les limites du tableau et contient un élément non NULL
    */
    bool posValide(Pos3d p)
    {
        return posValide(p.x,p.y,p.z);
    }

    bool posValide(int x, int y, int z)
    {
        try
        {
            if(get(x, y, z)==NULL) return false;
        }
        catch ( const int * e )
        {
            delete e;
            e=NULL;
            return false;
        }

        return true;
    }

	T* get(int x, int y, int z)
	{
		try
		{
			if(!data)
			{
				throw new int( 11 );
			}
			else
			{
				if(x<0 || x>= width || y < 0 || y>= height || z < 0 || z >= depth)
				{
					throw new int( 10 );
				}
				else
				{
					return data[x][y][z];
				}
			}
		}
		catch ( const int * e )
		{
			#ifdef __DEBUG__
				if(*e==10)
				{
					std::cerr << "Position [" << x << "," <<y << "," << z<< "] out of bounds" << " (Error " << *e << ")"<< endl;
				}
				else if(*e==11)
				{
					std::cerr << "Data table is not initialized"<< endl;
				}
			#endif

			throw;
		}
	}

	T* get(const Pos3d& pos)
	{
		try
		{
			get(pos.x, pos.y, pos.z);
		}
		catch(const int * e)
		{
			throw;
		}
	}

	void set(T* val,int x, int y, int z)
	{
		try
		{
			if(!data)
			{
				throw new int( 11 );
			}
			else
			{
				if(x<0 || x>= width || y < 0 || y>= height || z < 0 || z >= depth)
				{
					throw new int( 10 );
				}
				else
				{
					data[x][y][z] = val;
				}
			}
		}
		catch ( const int * e )
		{
			#ifdef __DEBUG__
				if(*e==10)
				{
					std::cerr << "Position [" << x << "," <<y << "," << z<< "] out of bounds" << " (Error " << *e << ")"<< endl;
				}
				else if(*e==11)
				{
					std::cerr << "Data table is not initialized"<< endl;
				}
			#endif
			throw;
		}
	}

    T* set(T* val, const Pos3d& pos)
	{
		try
		{
			this->set(val, pos.x, pos.y, pos.z);
		}
		catch(const int * e)
		{
			throw;
		}
	}

	Pos3d getPosVoisin(const Pos3d& pos,const Vec3d& direction)
	{
		Pos3d tmp = pos+direction;
		try
		{
			if(!data)
			{
				throw new int( 11 );
			}
			else
			{

				if(bEnDehors(tmp))
				{
					throw new int( 10 );
				}
				else
				{

					return tmp;
				}
			}
		}
		catch ( const int * e )
		{
			#ifdef __DEBUG__
				if(*e==10)
				{
					std::cerr << "Destination [" << tmp.x << "," <<tmp.y << "," << tmp.z<< "] out of bounds" << " (Error " << *e << ")"<< endl;
				}
				else if(*e==11)
				{
					std::cerr << "Data table is not initialized"<< endl;
				}
			#endif
			throw;
		}
	}

	Pos3d getPosVoisin(const Pos3d& pos,const enum Direction direction)
	{
		return getPosVoisin(pos, Voisins[direction]);
	}

	bool bEnDehors(const Pos3d& p)
	{
		if(p.x<0 || p.x>= width || p.y < 0 || p.y>= height || p.z < 0 || p.z >= depth)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	T* getVoisin(const Pos3d& pos,const Vec3d& direction)
	{
		try
		{
			Pos3d tmp = getPosVoisin(pos, direction);
			return this->get(tmp);
		}
		catch(const int * e)
		{
		    delete e;
			return NULL;
		}
	}

	T* getVoisin(const Pos3d& pos,const enum Direction direction)
	{
		return getVoisin(pos, Voisins[direction]);
	}

};

#endif
