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
 
#ifndef __INVENTORY_HPP__
#define __INVENTORY_HPP__

#define _INVENTORY_CELL_WIDTH_ 32
#define _INVENTORY_CELL_HEIGHT_ 32

#define _TYPE_GENERIC_ -1

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include "table3d.hpp"
#include "engine.hpp"

using namespace std;

class InventoryObject
{
    public:
    vector<Pos3d> mask;
    int type;
    char sous_type;
    GLuint tex;
    float w;
    float h;
    int cell_width;
    int cell_height;

    int x;
    int y;

    InventoryObject(int _cell_width = _INVENTORY_CELL_WIDTH_, int _cell_height = _INVENTORY_CELL_HEIGHT_, int _type=0, char _sous_type='a', string cheminTexture="", string _mask="x");

    ~InventoryObject();

    void draw(float pos_x, float pos_y, float alpha = 1.0);
    void drawInSlot(float pos_x, float pos_y, float alpha = 1.0);

    void trace();

    void copyToClipBoard(stringstream &ss);

    bool hit(Pos3d pos);

};


class Slot
{
    public:
    int type;
    float x;
    float y;
    float width;
    float height;
    GLuint tex;
    GLuint texOK;
    GLuint texKO;

    InventoryObject *obj;

    Slot(int _type, float _x, float _y, float _width, float _height, string imageFond="", string imageFondOK="", string imageFondKO="");

    ~Slot();

    void draw(int mouse_x, int mouse_y, InventoryObject* object);

    bool hit(float _x, float _y);
};

class Inventory : public Table3d<InventoryObject>
{
    public:

    GLuint tex_vide;
    GLuint tex_pleine;
    GLuint tex_ok;
    GLuint tex_ko;
    Engine* engine;
    float pos_x;
    float pos_y;

    int cell_width;
    int cell_height;

    vector<InventoryObject*> objectList;
    vector<Slot*> slotList;
    InventoryObject * holded;

    Inventory(int _width, int _height, float _pos_x, float _pos_y, int _cell_width, int _cell_height, Engine* _engine);

    ~Inventory();

    void addSlot(Slot* slot);

    /**
        Vérifie que l'objet à la place d'être positionné à la position indiquée
    */
    bool posOk(int x, int y,const InventoryObject *objet);

    /**
        Vérifie que l'objet à la place d'être positionné à la position indiquée si on l'échange avec un objet présent
    */
    InventoryObject * posOkEchange(int x, int y,const InventoryObject *objet, bool &possible);

    InventoryObject * addObjectEchange(int x, int y, InventoryObject *objet, bool &ok);
    /**
        Ajoute un objet à la position indiquée s'il y a la place

    */
    bool addObject(int x, int y, InventoryObject *objet);

    /**
        Ajoute un objet si possible (recherche automatique d'un emplacement
    */
    bool addObject(InventoryObject *objet);
    bool addObject(InventoryObject *objet, bool randomly);

    /**
        Retire l'objet donné en paramètre
    */
    InventoryObject * removeObject(InventoryObject *objet);

    /**
        Retire l'objet situé à la position donnée en paramètre
    */
    InventoryObject * removeObject(int x, int y);

    /**
        Regarde si l'objet tenu à la souris entre en collision avec la case donnée
    */
    bool holdedObjectPresent(int x, int y);

    Slot* getCorrespondingSlot(InventoryObject *obj);

    void draw(int mouse_x, int mouse_y, bool drawSlots=true, bool drawHolded=true);


    bool hitSlot();
    bool rightClickSlot();
	void eventManagement(SDL_Event &event, bool bAutoriseSuppression);
	void eventManagementToolMode(SDL_Event &event);

	Pos3d getPosition(float m_x, float m_y);
};

#endif
