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
 
#include "inventory.hpp"

// class InventoryObject


InventoryObject::InventoryObject(int _cell_width, int _cell_height, int _type, char _sous_type, string cheminTexture, string _mask)
    :type(_type), sous_type(_sous_type), cell_width(_cell_width), cell_height(_cell_height)
{
    this->x = 0;
    this->y = 0;

    int _x = 0;
    int _y = 0;
    w = 0;
    h = 1;
    for(int i = 0; i < _mask.size();++i)
    {
        if(_mask[i]=='\n')
        {
            _x=0;
            _y++;
            h++;
            w=0;
        }
        else if(_mask[i]=='x')
        {
            mask.push_back(Pos3d(_x,_y,0));
            _x++;
            w++;
        }
        else
        {
            _x++;
            w++;
        }
    }

    // On multiplie la largeur trouvée par la largeur d'une case pour avoir la largeur de la texture à afficher
    w*= _cell_width;
    h*= _cell_height;


    SDL_Surface* img =IMG_Load(cheminTexture.c_str());
    tex = SurfaceToTexture2(img);
    SDL_FreeSurface(img); img = NULL;
}

InventoryObject::~InventoryObject()
{
}

void InventoryObject::draw(float pos_x, float pos_y, float alpha)
{
    drawImage(tex,pos_x + (float)x * cell_width,pos_y + (float)y * cell_height,w,h,alpha);
}

void InventoryObject::drawInSlot(float pos_x, float pos_y, float alpha)
{
    drawImage(tex,pos_x,pos_y ,w,h,alpha);
}

void InventoryObject::trace()
{

}

void InventoryObject::copyToClipBoard(stringstream &ss)
{

}

bool InventoryObject::hit(Pos3d pos)
{
    std::vector<Pos3d>::iterator it;
    it = find(mask.begin(), mask.end(), pos);

    return !(it == mask.end());
}


// class Slot

Slot::Slot(int _type, float _x, float _y, float _width, float _height, string imageFond, string imageFondOK, string imageFondKO)
    :type(_type),x(_x), y(_y), width(_width), height(_height), obj(NULL)
{
    SDL_Surface* img =IMG_Load(imageFond.c_str());
    tex = SurfaceToTexture2(img);
    SDL_FreeSurface(img); img = NULL;

    img =IMG_Load(imageFondOK.c_str());
    texOK = SurfaceToTexture2(img);
    SDL_FreeSurface(img); img = NULL;

    img =IMG_Load(imageFondKO.c_str());
    texKO = SurfaceToTexture2(img);
    SDL_FreeSurface(img); img = NULL;
}

Slot::~Slot()
{

}

void Slot::draw(int mouse_x, int mouse_y, InventoryObject* object)
{
    bool bHit = hit(mouse_x,mouse_y);
    GLuint tmpTex;
    if(!bHit)
    {
        tmpTex=this->tex;
    }
    else if(object==NULL)
    {
        tmpTex=this->tex;
    }
    else if(this->type == _TYPE_GENERIC_ || object->type==this->type)
    {
       tmpTex=texOK;
    }
    else
    {
        tmpTex=texKO;
    }

    drawImage(tmpTex, x, y, width, height,1.0);

    if(obj!=NULL)
    {
        float tmp_x = x+(this->width-obj->w)/2.0;
        float tmp_y = y+(this->height-obj->h)/2.0;
        obj->drawInSlot(tmp_x, tmp_y);
    }
}

bool Slot::hit(float _x, float _y)
{
    if(_x>=x && _x <x+width && _y>=y && _y<y+height)
    {
        return true;
    }
    return false;
}



// class Inventory : public Table3d<InventoryObject>


Inventory::Inventory(int _width, int _height, float _pos_x, float _pos_y, int _cell_width, int _cell_height, Engine* _engine)
    :Table3d(_width, _height,1, false), pos_x(_pos_x), pos_y(_pos_y), cell_width(_cell_width), cell_height(_cell_height), engine(_engine)
{
    holded = NULL;
    SDL_Surface* img_vide =IMG_Load("themes/default/textures/caseInvVide.png");
    tex_vide = SurfaceToTexture2(img_vide);
    SDL_FreeSurface(img_vide); img_vide = NULL;

    SDL_Surface* img_pleine =IMG_Load("themes/default/textures/caseInvPleine.png");
    tex_pleine = SurfaceToTexture2(img_pleine);
    SDL_FreeSurface(img_pleine); img_pleine = NULL;

    SDL_Surface* img_OK =IMG_Load("themes/default/textures/caseInvOK.png");
    tex_ok = SurfaceToTexture2(img_OK);
    SDL_FreeSurface(img_OK); img_OK = NULL;

    SDL_Surface* img_KO =IMG_Load("themes/default/textures/caseInvKO.png");
    tex_ko = SurfaceToTexture2(img_KO);
    SDL_FreeSurface(img_OK); img_KO = NULL;
}

Inventory::~Inventory()
{
    int nbObjets = objectList.size();
    for(int i = 0; i < nbObjets; ++i)
    {
        removeObject(objectList[0]);
    }
    delete holded; holded = NULL;
}

void Inventory::addSlot(Slot* slot)
{
    slotList.push_back(slot);
}

bool Inventory::posOk(int x, int y,const InventoryObject *objet)
{
    // Pour chaque position du masque de l'objet à tester,
    // on regarde si la place est libre
    for(int i=0;i<objet->mask.size();++i)
    {
        // on calcul la position de la case a tester en fonction de la position de départ et du masque
        Pos3d pos_tmp(x+objet->mask[i].x,y+objet->mask[i].y,0);

        try
        {
            // S'il y a un objet à cette position, on ne peut pas insérer l'objet
            if(this->get(pos_tmp)!=NULL) return false;
        }
        catch( const int * e )
        {
            // Un des éléments du masque est à l'extérieur du tableau
            delete e;
            return false;
        }

    }
    return true;
}

InventoryObject * Inventory::posOkEchange(int x, int y,const InventoryObject *objet, bool &possible)
{
    InventoryObject * objetPresent = NULL;
    // Pour chaque position du masque de l'objet à tester,
    // on regarde si la place est libre
    for(int i=0;i<objet->mask.size();++i)
    {
        // on calcul la position de la case à tester en fonction de la position de départ et du masque
        Pos3d pos_tmp(x+objet->mask[i].x,y+objet->mask[i].y,0);

        try
        {
            InventoryObject *obj = this->get(pos_tmp);
            if(obj != NULL)
            {
                if(objetPresent==NULL)
                {
                    objetPresent = obj;
                }
                else
                {
                    // S'il y a plusieurs objets présents à l'endroit ou on veut poser l'objet, on ne pourra pas le poser
                    if(obj!=objetPresent)
                    {
                        possible = false;
                        return NULL;
                    }
                }
            }
        }
        catch( const int * e )
        {
            // Un des éléments du masque est à l'extérieur du tableau
            delete e;
            possible = false;
            return NULL;
        }

    }
    possible = true;
    return objetPresent;
}

bool Inventory::addObject(int x, int y, InventoryObject *objet)
{
    if(posOk(x, y, objet))
    {
        objet->x = x;
        objet->y = y;
        // On insert un pointeur vers l'objet pour chaque position du masque
        for(int i=0;i<objet->mask.size();++i)
        {
            Pos3d pos_tmp(x+objet->mask[i].x,y+objet->mask[i].y,0);
            this->set(objet, pos_tmp);
        }
        objectList.push_back(objet);
        return true;
    }
    else
    {
        return false;
    }
}

InventoryObject * Inventory::addObjectEchange(int x, int y, InventoryObject *objet, bool &ok)
{
    InventoryObject *present=posOkEchange(x, y, objet, ok);

    if(ok)
    {
        if(present!=NULL) removeObject(present);

        objet->x = x;
        objet->y = y;
        // On insert un pointeur vers l'objet pour chaque position du masque
        for(int i=0;i<objet->mask.size();++i)
        {
            Pos3d pos_tmp(x+objet->mask[i].x,y+objet->mask[i].y,0);
            this->set(objet, pos_tmp);
        }
        objectList.push_back(objet);
        ok = true;
        return present;
    }
    else
    {
        ok=false;
        return NULL;
    }
}

bool Inventory::addObject(InventoryObject *objet)
{
    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            if(addObject(x,y,objet))
            {
                return true;
            }
        }
    }
    return false;
}

bool Inventory::addObject(InventoryObject *object, bool randomly)
{
    if(!randomly)
    {
        return addObject(object);
    }
    else
    {
        vector<int> cases;
        for(int i = 0; i < this->width*this->height; ++i)
        {
            cases.push_back(i);
        }
        random_shuffle(cases.begin(), cases.end());

        for(int i = 0; i < this->width*this->height; ++i)
        {
            if(addObject(cases[i]/this->width, cases[i]%this->width, object)) return true;
        }
    }
    return false;
}

InventoryObject * Inventory::removeObject(InventoryObject *objet)
{
    bool ok = false;
    for(int y = 0; y < this->height;++y)
    {
        for(int x = 0; x < this->width;++x)
        {
            if(this->get(x,y,0) == objet)
            {
                ok = true;
                this->set(NULL, x, y, 0);
            }
        }
    }

    if(ok)
    {
        for(int i = 0;i<objectList.size();++i)
        {
            if(objectList[i] == objet)
            {
                objectList.erase(objectList.begin()+i);
                break;
            }
        }

        objet->x = 0;
        objet->y = 0;
        return objet; // retourne l'objet retiré
    }
    else
    {
        return NULL; // renvoi NULL si l'objet n'était pas présent dans l'inventaire
    }
}

InventoryObject * Inventory::removeObject(int x, int y)
{
    InventoryObject *obj = NULL;
    try
    {
        obj = this->get(x,y,0);
    }
    catch( const int * e )
    {
        delete e;
        return NULL;
    }
    if(obj!=NULL)
    {
        return removeObject(obj);
    }

    return NULL;
}

bool Inventory::holdedObjectPresent(int x, int y)
{
    if(!this->holded) return false;

    Pos3d posSouris = getPosition(engine->m_x, engine->m_y);
    Pos3d pos = Pos3d(x-posSouris.x,y-posSouris.y,0);
    return holded->hit(pos);

}

void Inventory::draw(int mouse_x, int mouse_y, bool drawSlots, bool drawHolded)
{
    if(drawSlots)
    {
        for(int i =0; i < slotList.size();++i)
        {
            slotList[i]->draw(mouse_x, mouse_y, holded);
        }
    }

    Pos3d posSouris = getPosition(engine->m_x, engine->m_y);
    bool positionOk = (holded != NULL);
    if(positionOk)
    {
        this->posOkEchange(posSouris.x, posSouris.y,holded,positionOk);
    }

    // On masque le curseur de la souris si on porte un objet
    if(holded != NULL)
    {
        SDL_ShowCursor(SDL_DISABLE);
    }
    else
    {
        SDL_ShowCursor(SDL_ENABLE);
    }

    for(int y=0; y < this->height;++y)
    {
        for(int x=0; x < this->width;++x)
        {
            GLuint tmp_tex;

            if(data[x][y][0]!=NULL)
            {
                if(holdedObjectPresent(x, y))
                {
                    if(positionOk)
                    {
                        tmp_tex = tex_ok;
                    }
                    else
                    {
                        tmp_tex = tex_ko;
                    }
                }
                else
                {
                    tmp_tex = tex_pleine;
                }
            }
            else
            {
                if(holdedObjectPresent(x, y))
                {
                    if(positionOk)
                    {
                        tmp_tex = tex_ok;
                    }
                    else
                    {
                        tmp_tex = tex_ko;
                    }
                }
                else
                {
                    tmp_tex = tex_vide;
                }
            }

            drawImage(tmp_tex,pos_x + (float)x * this->cell_width,pos_y + (float)y * this->cell_height,this->cell_width,this->cell_height,1.0);
        }
    }

    for(int i = 0; i < this->objectList.size();++i)
    {
        objectList[i]->draw(pos_x,pos_y);
    }

    if(holded != NULL && drawHolded)
    {
        holded->draw(engine->m_x, engine->m_y,0.7);
    }
}


bool Inventory::hitSlot()
{
    for(int i =0; i < slotList.size();++i)
    {
        if(slotList[i]->hit(engine->m_x, engine->m_y))
        {
            if(holded != NULL)
            {
                if(slotList[i]->type == _TYPE_GENERIC_ || slotList[i]->type == holded->type)
                {
                    InventoryObject* tmp = slotList[i]->obj;
                    slotList[i]->obj = holded;
                    holded = tmp;
                }
            }
            else if(holded == NULL && slotList[i]->obj!=NULL)
            {
                holded = slotList[i]->obj;
                slotList[i]->obj = NULL;
            }

            return true;
        }
    }
    return false;
}

bool Inventory::rightClickSlot()
{
    for(int i =0; i < slotList.size();++i)
    {
        if(slotList[i]->hit(engine->m_x, engine->m_y))
        {
            if(holded == NULL && slotList[i]->obj!=NULL)
            {
                // Si on arrive a mettre l'objet dans l'inventaire, on le retire du slot
                if(addObject(slotList[i]->obj,true))
                {
                    slotList[i]->obj = NULL;
                }
            }

            return true;
        }
    }
    return false;
}

void Inventory::eventManagement(SDL_Event &event, bool bAutoriseSuppression)
{

    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                if(!hitSlot())
                {
                    Pos3d tmp = getPosition(engine->m_x, engine->m_y);
                    if(holded==NULL)
                    {
                        if(SDL_GetModState() & KMOD_CTRL)
                        {
                            if(bAutoriseSuppression)
                            {
                                delete this->removeObject(tmp.x, tmp.y);
                            }
                        }
                        else
                        {
                            holded = this->removeObject(tmp.x, tmp.y);
                        }
                    }
                    else
                    {
                        bool ok;
                        InventoryObject* obj = this->addObjectEchange(tmp.x, tmp.y,holded,ok);
                        if(ok)
                        {
                            holded = obj;
                        }
                    }
                }
            }

            if(event.button.button == SDL_BUTTON_RIGHT)
            {
                // Equiper/déséquiper les items automatiquement via le clique droit
                if(holded==NULL)
                {
                    // Renvoi de l'objet du slot dans l'inventaire s'il y a la place
                    if(!rightClickSlot())
                    {
                        // Equiper l'objet dans son slot correspondant si possible
                        Pos3d tmp = getPosition(engine->m_x, engine->m_y);
                        try
                        {
                            InventoryObject* obj = this->get(tmp.x,tmp.y,0);
                            if(obj!=NULL)
                            {
                                Slot* tmpSlot = getCorrespondingSlot(obj);
                                if(tmpSlot!=NULL)
                                {
                                    // Si le slot est vide, on se contente d'insérer l'objet
                                    if(tmpSlot->obj==NULL)
                                    {
                                        tmpSlot->obj = obj;
                                        removeObject(obj);
                                    }
                                    else
                                    {
                                        // Si le slot contient déjà un objet, on essai d'échanger l'objet du slot et celui de l'inventaire
                                        int tmp_x=obj->x;
                                        int tmp_y=obj->y;
                                        removeObject(obj);
                                        if(addObject(tmpSlot->obj))
                                        {
                                            tmpSlot->obj = obj;
                                        }
                                        else
                                        {
                                            addObject(tmp_x,tmp_y,obj);
                                        }

                                    }
                                }
                            }
                        }
                        catch(const int * e )
                        {
                            delete e;
                            e=NULL;
                        }


                    }
                }
            }
        break;
    }
}

void Inventory::eventManagementToolMode(SDL_Event &event)
{

    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
            {
                // Equiper les items automatiquement via le clique gauche


                // Equiper l'objet dans son slot correspondant
                Pos3d tmp = getPosition(engine->m_x, engine->m_y);
                try
                {
                    InventoryObject* obj = this->get(tmp.x,tmp.y,0);
                    if(obj!=NULL)
                    {
                        Slot* tmpSlot = getCorrespondingSlot(obj);
                        if(tmpSlot!=NULL)
                        {
                            tmpSlot->obj = obj;
                        }
                    }
                }
                catch(const int * e )
                {
                    delete e;
                    e=NULL;
                }



            }
        break;
    }
}

Slot* Inventory::getCorrespondingSlot(InventoryObject *obj)
{
    if(obj==NULL) return NULL;

    for(int i = 0; i < slotList.size(); ++i)
    {
        if(obj->type==slotList[i]->type) return slotList[i];
    }

    return NULL;
}

Pos3d Inventory::getPosition(float m_x, float m_y)
{
    Pos3d ret(0,0,0);
    int x=(m_x-pos_x);
    int y=(m_y-pos_y);

    ret.x = x/this->cell_width;
    ret.y = y/this->cell_height;

    if(x<0) ret.x--;
    if(y<0) ret.y--;

    return ret;
}
