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
 
#include <cstring>
#include "sprite2d.hpp"


/********** HITBOX2D *************/
Hitbox2d::Hitbox2d(Engine *_engine, enum TypeHitbox _Type, int _group)
	:engine(_engine), m_Type(_Type), group(_group)
{
}

Hitbox2d::~Hitbox2d()
{
}

enum TypeHitbox Hitbox2d::getType() const
{
	return m_Type;
}
/********** HITBOX2DCIRCLE *************/
Hitbox2dCircle::Hitbox2dCircle(Engine *_engine)
	: Hitbox2d(_engine, CIRCLE)
{
}

Hitbox2dCircle::~Hitbox2dCircle()
{
}

bool Hitbox2dCircle::bLoadXML(IrrXMLReader* xml)
{
	m_position.X = atoi(xml->getAttributeValue("x1"));
	m_position.Y = atoi(xml->getAttributeValue("y1"));
	if(xml->getAttributeValue("group")!=NULL)
    {
        this->group = atoi(xml->getAttributeValue("group"));
    }
	m_Radius = atoi(xml->getAttributeValue("radius"));

	return true;
}

bool Hitbox2dCircle::bCollision(Hitbox2d* hb2d, position2d<int> offset, position2d<int> offsethb2d) const
{

	if(hb2d->getType() == CIRCLE)
	{
		position2d<int> poshb1;
		position2d<int> poshb2;

		poshb1.X = this->m_position.X + offset.X;
		poshb1.Y = this->m_position.Y + offset.Y;

		poshb2.X = dynamic_cast<Hitbox2dCircle*>(hb2d)->m_position.X + offsethb2d.X;
		poshb2.Y = dynamic_cast<Hitbox2dCircle*>(hb2d)->m_position.Y + offsethb2d.Y;

		if((poshb1 - poshb2).getLength()
				< (dynamic_cast<Hitbox2dCircle*>(hb2d)->m_Radius + this->m_Radius))
		{
			//cout << "OK" << endl;
			//cout << offset.X << ", " << offset.Y << endl;
			//cout << offsethb2d.X << ", " << offsethb2d.Y << endl;
			//cout << (poshb1 - poshb2).getLength()  << ", " << (dynamic_cast<Hitbox2dCircle*>(hb2d)->m_Radius + this->m_Radius)<<endl;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// TODO : collision with Rectangle Hitbox
		return false;
	}
}


void Hitbox2dCircle::vDraw(position2d<int> offset) const
{
	position2d<int> center = m_position+offset;
	// dessiner un cercle correspondant à la hitbox
	//m_pDriver->draw2DPolygon(center, m_Radius);
	return;
}

/********** HITBOX2DRECTANGLE *************/
void Hitbox2dRectangle::vDraw(position2d<int> offset) const
{
	SDL_Rect rectangle = m_Rect;
	rectangle.x += offset.X;
	rectangle.y += offset.Y;
	// Dessiner le rectangle à l'écran
	//m_pDriver->draw2DRectangleOutline(rectangle);
	return;
}

Hitbox2dRectangle::Hitbox2dRectangle(Engine *_engine)
	: Hitbox2d(_engine, RECTANGLE)
{
}

Hitbox2dRectangle::~Hitbox2dRectangle()
{
}

bool Hitbox2dRectangle::bLoadXML(IrrXMLReader* xml)
{
	int x1,y1,x2,y2;
	x1 = atoi(xml->getAttributeValue("x1"));
	y1 = atoi(xml->getAttributeValue("y1"));
	x2 = atoi(xml->getAttributeValue("x2"));
	y2 = atoi(xml->getAttributeValue("y2"));
	m_Rect.x = x1;
	m_Rect.y = y1;
	m_Rect.w = x2-x1;
	m_Rect.h = y2-y1;

    if(xml->getAttributeValue("group")!=NULL)
    {
        this->group = atoi(xml->getAttributeValue("group"));
    }

	return true;
}

bool Hitbox2dRectangle::bCollision(Hitbox2d* hb2d, position2d<int> offset, position2d<int> offsethb2d) const
{
    if(hb2d->getType()==RECTANGLE)
    {
        // Collision with rectangle hitbox
        int centreX = this->m_Rect.x+m_Rect.w/2.0 + offset.X;
        int centreY = this->m_Rect.y+m_Rect.h/2.0 + offset.Y;
        int centreX2 = dynamic_cast<Hitbox2dRectangle*>(hb2d)->m_Rect.x+dynamic_cast<Hitbox2dRectangle*>(hb2d)->m_Rect.w/2.0 + offsethb2d.X;
        int centreY2 = dynamic_cast<Hitbox2dRectangle*>(hb2d)->m_Rect.y+dynamic_cast<Hitbox2dRectangle*>(hb2d)->m_Rect.h/2.0 + offsethb2d.Y;
        int distX = abs( centreX - centreX2);
        int distY = abs( centreY - centreY2);
        if(distX < (this->m_Rect.w +  dynamic_cast<Hitbox2dRectangle*>(hb2d)->m_Rect.w)/2.0
           && distY < (this->m_Rect.h +  dynamic_cast<Hitbox2dRectangle*>(hb2d)->m_Rect.h)/2.0)
        {
            return true;
        }
    }
    else if(hb2d->getType()==CIRCLE)
    {
        // TODO : collision with circle hitbox
    }
	return false;
}

/********** KEYFRAME2D *************/
KeyFrame2d::KeyFrame2d(Engine *_engine, Animation2d* _pAnimation2d, ThemeFileFinder* file_finder)
    :engine(_engine), m_pAnimation2d(_pAnimation2d)
{
    this->file_finder = file_finder;
    this->m_texture = NULL;
	//cout << "Keyframe2d constructeur" << endl;
    m_iLastFrame = _pAnimation2d->iGetDuration();
	//cout << "Keyframe2d constructeur2" << endl;
}

KeyFrame2d::~KeyFrame2d()
{
    this->m_texture = NULL;
}

bool KeyFrame2d::bLoadXML(IrrXMLReader* xml)
{
//cout << "load keyframe2d" << endl;
    bool bSuccess = true;
//	cout << xml->getAttributeValue("filename") << endl;

    string chemin_complet;
	chemin_complet=file_finder->find(REP_TEXTURES,xml->getAttributeValue("filename"));

	this->m_texture = engine->texturePool.getTexture(xml->getAttributeValue("filename"));
	//this->m_texture->charger("SOL.jpg",this->file_finder);

// cout << "load keyframe2d" << endl;
    m_iDuration = atoi(xml->getAttributeValue("duration"));
    m_iLastFrame += m_iDuration;
//  cout << "load keyframe2d" << endl;
    m_SourceRect.x = atoi(xml->getAttributeValue("src_x1"));
    m_SourceRect.y = atoi(xml->getAttributeValue("src_y1"));
    m_SourceRect.w = atoi(xml->getAttributeValue("src_x2")) - m_SourceRect.x;
    m_SourceRect.h = atoi(xml->getAttributeValue("src_y2")) - m_SourceRect.y;
//  cout << "load keyframe2d" << endl;
    m_iOffset_x = atoi(xml->getAttributeValue("offset_x"));
    m_iOffset_y = atoi(xml->getAttributeValue("offset_y"));
//cout << "load keyframe2d" << endl;
	while(xml->read())
	{
        switch(xml->getNodeType())
        {
            case EXN_TEXT:
                // ignored
            break;
            case EXN_ELEMENT:
				if(strcmp(xml->getNodeName(),"hitbox")==0)
                {
                    Hitbox2d* hitbox = NULL;

					if(strcmp(xml->getAttributeValue("type"),"circle")==0)
					{
						// Hitbox Circulaire
						hitbox = new Hitbox2dCircle(engine);
					}
					else
					{
						// Hitbox Rectangulaire
						hitbox = new Hitbox2dRectangle(engine);
					}
						bSuccess = hitbox->bLoadXML(xml);
						if(bSuccess)
						{
	                    	m_HitboxList.push_back(hitbox);
						}
                }
                else
				{
					return bSuccess;
				}
			break;
		}
	}
    return bSuccess;
}

int KeyFrame2d::iGetDuration()
{
    return m_iDuration;
}

int KeyFrame2d::iGetLastFrame()
{
    return m_iLastFrame;
}

void KeyFrame2d::vDrawOpenGL(position2d<int> dest,bool h_flip)
{
    float t[8];

    this->appliquer(t, h_flip);

    SDL_Rect destRect;
    destRect.x = dest.X + m_iOffset_x;
    destRect.y = dest.Y + m_iOffset_y;
	destRect.w = m_SourceRect.w;
	destRect.h = m_SourceRect.h;

    glBegin(GL_QUADS);

    glTexCoord2f (t[0], t[1]);
    glVertex2f(destRect.x, destRect.y);

    glTexCoord2f (t[2], t[3]);
    glVertex2f(destRect.x+destRect.w, destRect.y);

    glTexCoord2f (t[4], t[5]);
    glVertex2f(destRect.x+destRect.w, destRect.y+destRect.h);

    glTexCoord2f (t[6], t[7]);
    glVertex2f(destRect.x, destRect.y+destRect.h);
    glEnd();
}

void KeyFrame2d::appliquer(float t[8],bool h_flip)
{
	// vérifie que la texture existe
	if(m_texture == NULL) return;

   	m_texture->appliquer();


    float x1 = (float)(m_SourceRect.x+m_iOffset_x)/m_texture->w;
    float y1 = (float)(m_SourceRect.h + m_SourceRect.y + m_iOffset_y)/m_texture->h;
	float x2 = (float)(m_SourceRect.w + m_SourceRect.x + m_iOffset_x)/m_texture->w;
    float y2 = (float)(m_SourceRect.y+m_iOffset_y)/m_texture->h;


    if(h_flip)
    {
        t[0] = x2; t[1] = y2;
        t[2] = x1; t[3] = y2;
        t[4] = x1; t[5] = y1;
        t[6] = x2; t[7] = y1;
    }
    else
    {
        t[0] = x1; t[1] = y2;
        t[2] = x2; t[3] = y2;
        t[4] = x2; t[5] = y1;
        t[6] = x1; t[7] = y1;
    }

	/*t[0] = 0.0; t[1] = 1.0;
	t[2] = 0.25; t[3] = 1.0;
    t[4] = 0.25; t[5] = 0.0;
	t[6] = 0.0; t[7] = 0.0;*/

}

std::ostream &operator << (std::ostream &out, const KeyFrame2d &keyFrame2d)
{

    out << "   " << setw(5) << keyFrame2d.m_iDuration << " ";
    out << "   " << setw(5) << keyFrame2d.m_SourceRect.x << " ";
    out << "   " << setw(5) << keyFrame2d.m_SourceRect.y << " ";
    out << "   " << setw(5) << keyFrame2d.m_SourceRect.x + keyFrame2d.m_SourceRect.w << " ";
    out << "   " << setw(5) << keyFrame2d.m_SourceRect.y + keyFrame2d.m_SourceRect.h << " ";
    out << "   " << setw(5) << keyFrame2d.m_iOffset_x << " ";
    out << "   " << setw(5) << keyFrame2d.m_iOffset_y << " ";
    out << "   " << setw(5) << keyFrame2d.m_iLastFrame << endl;

    return out;
}

bool KeyFrame2d::bCollision(KeyFrame2d* kf2d, position2d<int> offset, position2d<int> offsetkf2d)
{
	for(int i = 0; i < m_HitboxList.size();++i)
	{
		for(int j = 0; j < kf2d->m_HitboxList.size();++j)
		{
			if(m_HitboxList[i]->bCollision(kf2d->m_HitboxList[j], offset, offsetkf2d))
			{
				//cout << "KeyFrame COLLISION" << endl;
				return true;
			}
		}
	}

	return false;
}

/********** ANIMATION2D ************/
Animation2d::Animation2d(Engine *_engine, ThemeFileFinder* file_finder)
    :engine(_engine), m_iDuration(0)
{
    this->file_finder = file_finder;
}
Animation2d::~Animation2d()
{
}

bool Animation2d::bLoadXML(IrrXMLReader* xml)
{
    bool bSuccess = false;
	bool bSetByOther = false;
    m_bLoop = (!strcmp(xml->getAttributeValue("loop"),"ON")?true:false);
    m_sName = xml->getAttributeValue("name");

    while(bSetByOther || xml->read())
    {
		bSetByOther = false;
        switch(xml->getNodeType())
        {
            case EXN_TEXT:
                // ignored
            break;
            case EXN_ELEMENT:

                if(strcmp(xml->getNodeName(),"keyframe")==0)
                {
					//cout << "animation2d" << endl;
                    KeyFrame2d* keyframe = new KeyFrame2d(engine, this, this->file_finder);
                    m_KeyFrame2dList.push_back(keyframe);
                    bSuccess = keyframe->bLoadXML(xml);
					bSetByOther = bSuccess;
                    if(bSuccess)
                    {
                        m_iDuration+=keyframe->iGetDuration();
                    }
                }
                else
                {
                    // end of animation nodes
                    return bSuccess;
                }
            break;
        }
    }

    return bSuccess;
}

void Animation2d::vUpdate(SpriteState& state)
{
    if(state.m_bRunning)
    {
        state.m_iCurrentKeyFrame = 0;
        unsigned int DeltaTime;
		DeltaTime = (state.m_pTimer->getTime() - state.m_iBeginningTime);
        if(m_bLoop)
		{
			DeltaTime = DeltaTime%m_iDuration;
		}
		else
		{
			if(DeltaTime > m_iDuration) DeltaTime = m_iDuration;
		}

        while(DeltaTime > m_KeyFrame2dList[state.m_iCurrentKeyFrame]->iGetLastFrame())
        {
            ++state.m_iCurrentKeyFrame;
        }
    }
}

void Animation2d::vDrawOpenGL(const SpriteState& state,bool h_flip)
{
	//cout << "Draw Animation " << endl;
	//cout << "NB frames : " << m_KeyFrame2dList.size() << endl;
	//cout << "Current KeyFrame : " << state.m_iCurrentKeyFrame << endl;
	//cout << m_KeyFrame2dList[state.m_iCurrentKeyFrame] << endl;
    m_KeyFrame2dList[state.m_iCurrentKeyFrame]->vDrawOpenGL(*(state.m_position),h_flip);
}

void Animation2d::appliquer(const SpriteState& state, float t[8])
{
	m_KeyFrame2dList[state.m_iCurrentKeyFrame]->appliquer(t);
}

std::string Animation2d::sGetName()
{
    return m_sName;
}

int Animation2d::iGetDuration()
{
    return m_iDuration;
}

KeyFrame2d* Animation2d::pGetKeyFrame(const SpriteState &state)
{
	return m_KeyFrame2dList[state.m_iCurrentKeyFrame];
}

bool Animation2d::bLastFrame(const SpriteState &state)
{
	return (pGetKeyFrame(state) == m_KeyFrame2dList[m_KeyFrame2dList.size()-1]);
}

std::ostream &operator << (std::ostream &out, const Animation2d &animation2d)
{
    out << " " << animation2d.m_sName << " : "<<endl;
    out << "   " << setw(5) << "dur" << " ";
    out << "   " << setw(5) << "x1" << " ";
    out << "   " << setw(5) << "y1" << " ";
    out << "   " << setw(5) << "x2" << " ";
    out << "   " << setw(5) << "y2" << " ";
    out << "   " << setw(5) << "Offx" << " ";
    out << "   " << setw(5) << "Offy" << " ";
    out << "   " << setw(5) << "Last" <<endl;
    for(int i = 0; i < animation2d.m_KeyFrame2dList.size(); ++i)
    {
        out << "  " << *(animation2d.m_KeyFrame2dList[i]) << endl;
    }
    return out;
}

/********** ANIMATIONSET ************/
AnimationSet::AnimationSet(Engine *_engine, ThemeFileFinder* file_finder)
{
    this->file_finder =file_finder;
	engine = _engine;
	//cout  << "AnimationSet constructeur" << endl;
}
AnimationSet::~AnimationSet()
{
}

bool AnimationSet::bLoadXML(const std::string& dossier, const std::string& file)
{
	std::string chemin = dossier + file;
	//cout << "Chemin : " << chemin << endl;
    IrrXMLReader* xml;
    bool bSuccess = false;
    bool bSetByOther = false;
    if(chemin != "")
    {
        xml = createIrrXMLReader(chemin.c_str());
        if(xml)
        {
            while(bSetByOther || xml->read())
            {
                bSuccess = true;
                bSetByOther=false;
                switch(xml->getNodeType())
                {
                    case EXN_TEXT:
                        // ignored
                    break;
                    case EXN_ELEMENT:
                        if (strcmp("sprite", xml->getNodeName())==0)
                            m_sName = xml->getAttributeValue("name");
                        else
                        if (strcmp("animation", xml->getNodeName())==0)
                        {
							//cout << "aaaaaaaaa" << endl;
                            Animation2d* anim = new Animation2d(engine, file_finder);
							//cout << "aaaaaaaaa" << endl;
                            bSetByOther = anim->bLoadXML(xml);
							//cout << "bbbbbbbbb" << endl;
                            if(bSetByOther)
                            {
                                //m_Animation2dList.push_back(anim);
                                m_Animation2dList[anim->sGetName()] = anim;
                            }
                            else
                            {
                                delete anim;
                                anim = NULL;
                            }
                        }
                    break;
                }
            }
        }

        delete xml;
        xml = NULL;

    }

    return bSuccess;
}

bool AnimationSet::bSetAnimation(SpriteState& state, const std::string& _animationName)
{
    if(m_Animation2dList[_animationName]!= NULL)
    {
        state.m_sCurrentAnimation = m_Animation2dList[_animationName]->sGetName();
        state.m_iCurrentKeyFrame = 0;
        state.m_iBeginningTime = state.m_pTimer->getTime();
        state.m_bRunning = true;
        return true;
    }

    return false;
}

std::string AnimationSet::sGetName()
{
    return m_sName;
}

KeyFrame2d* AnimationSet::pGetKeyFrame(const SpriteState &state)
{
	return m_Animation2dList[state.m_sCurrentAnimation]->pGetKeyFrame(state);
}

void AnimationSet::vUpdate(SpriteState& state)
{
	//cout << "AZERTY "<< state.m_sCurrentAnimation << endl;
    m_Animation2dList[state.m_sCurrentAnimation]->vUpdate(state);
}

void AnimationSet::vDrawOpenGL(SpriteState& state,bool h_flip)
{
	//cout << "Current Animation : " << state.m_iCurrentAnimation << endl;
    m_Animation2dList[state.m_sCurrentAnimation]->vDrawOpenGL(state,h_flip);
}

void AnimationSet::appliquer(SpriteState& state, float t[8])
{
	return m_Animation2dList[state.m_sCurrentAnimation]->appliquer(state,t);
}

bool AnimationSet::bLastFrame(const std::string sAnimationName, const SpriteState &state)
{
	if(sAnimationName == state.m_sCurrentAnimation)
	{
		return m_Animation2dList[state.m_sCurrentAnimation]->bLastFrame(state);
	}
	else
	{
		return false;
	}
}

/********** SPRITE2D ************/
Sprite2d::Sprite2d(Engine *_engine, Timer *_pTimer, ThemeFileFinder* file_finder)
{
    this->file_finder = file_finder;
	engine = _engine;
	m_state.m_pTimer = _pTimer;
	m_state.m_position = new position2d<int>(0,0);
	m_pAnimationSet = NULL;
}

Sprite2d::~Sprite2d()
{
}

bool Sprite2d::init(const std::string& _name, AnimationPool* pAnimationPool)
{
	if( pAnimationPool != NULL)
	{
		m_pAnimationSet = pAnimationPool->getAnimationSet(_name);
		//cout << "AnimationSet : " << m_pAnimationSet<<endl;
		if(m_pAnimationSet != NULL) return true;
	}

	return false;
}

bool Sprite2d::bSetAnimation(const std::string& _animationName)
{
	bool retour;
    if(m_pAnimationSet != NULL)
	{
		retour =  m_pAnimationSet->bSetAnimation(m_state, _animationName);
		//cout << "Animation : "<< m_state.m_iCurrentAnimation << endl;
		return retour;
	}
	else
	{
		return false;
	}
}

void Sprite2d::vSetPosition(position2d<int> dest)
{
	m_state.m_position = new position2d<int>(dest);
}

void Sprite2d::vMove(int x, int y)
{
	m_state.m_position->X += x;
	m_state.m_position->Y += y;
}

void Sprite2d::vStart()
{
	m_state.m_iCurrentKeyFrame = 0;
    m_state.m_iBeginningTime = m_state.m_pTimer->getTime();
    m_state.m_bRunning = true;
}

void Sprite2d::vStop()
{
	m_state.m_iCurrentKeyFrame = 0;
    m_state.m_iBeginningTime = m_state.m_pTimer->getTime();
    m_state.m_bRunning = false;
}

void Sprite2d::vUpdate()
{
    m_pAnimationSet->vUpdate(m_state);
}

void Sprite2d::vDrawOpenGL(bool h_flip)
{
	//cout << "Draw sprite" << endl;
	//cout << m_pAnimationSet << endl;
    m_pAnimationSet->vDrawOpenGL(m_state, h_flip);
}

void Sprite2d::appliquer(float t[8])
{
	m_pAnimationSet->appliquer(m_state,t);
}

KeyFrame2d* Sprite2d::pGetKeyFrame()
{
	return m_pAnimationSet->pGetKeyFrame(m_state);
}

bool Sprite2d::bCollision(Sprite2d* sprite2d)
{
	//cout << "Collision " << this << endl;
	//cout << sprite2d->getPosition().X << ", " << sprite2d->getPosition().Y << endl;
	//cout << this->getPosition().X << ", " << this->getPosition().Y << endl;
	return this->pGetKeyFrame()->bCollision(sprite2d->pGetKeyFrame(),
		this->getPosition(), sprite2d->getPosition());
}

position2d<int> Sprite2d::getPosition()
{
	return *(m_state.m_position);
}

/*
std::string Sprite2d::sGetAnimationName()
{

	if(m_pAnimationSet!=NULL)
	{
		return this->m_pAnimationSet->sGetAnimationName(m_state);
	}
	else
	{
		return "";
	}
}
*/

std::string Sprite2d::sGetAnimationName()
{
    return m_state.m_sCurrentAnimation;
}

bool Sprite2d::bLastFrame(const std::string sAnimationName)
{
	return this->m_pAnimationSet->bLastFrame(sAnimationName, this->m_state);
}

/*ostream & operator<<(std::ostream &out, const Sprite2d &sprite2d)
{
    out << sprite2d.m_sName << " : "<< endl;
    for(int i = 0; i < sprite2d.m_Animation2dList.size(); ++i)
    {
        out << " " << *(sprite2d.m_Animation2dList[i]) << endl;
    }

    return out;
}*/

/********** ANIMATIONPOOL ************/

AnimationPool::AnimationPool(Engine *_engine, const std::string& _dossier, ThemeFileFinder* file_finder)
	:m_cDossier(_dossier), engine(_engine)
{
    this->file_finder = file_finder;
}

AnimationPool::~AnimationPool()
{
	for(int i = 0; i < animationSetList.size(); i++)
	{
		delete animationSetList[i];
		animationSetList[i] = NULL;
	}
}

AnimationSet* AnimationPool::getAnimationSet(const std::string& name)
{
	for(int i = 0; i < animationSetList.size(); ++i)
	{
		//cout << animationSetList[i]->sGetName() << ", " << name << endl;
		if(animationSetList[i]->sGetName().compare(name) == 0)
		{
			//cout << "======== TROUVE ========" << endl;
			return animationSetList[i];
		}
	}
	//cout << "pool1" << endl;
	// Le set d'animation n'a pas été trouvé, on tente de le charger
	AnimationSet* tmp;
	tmp = new AnimationSet(engine, file_finder);

	if(tmp->bLoadXML(m_cDossier, name))
	{
		animationSetList.push_back(tmp);
		//cout << "pool2" << endl;
		return tmp;
	}
	else
	{
		delete tmp;
		tmp = NULL;
//cout << "pool3" << endl;
		return NULL;
	}

}
