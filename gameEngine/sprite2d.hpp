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
 
#ifndef __SPRITE2D_HPP__
#define __SPRITE2D_HPP__

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <irrXML.h>
#include "animation.hpp"
#include "engine.hpp"

#include "position2d.hpp"

#include "timer.hpp"

using namespace std;

enum TypeHitbox
{
	CIRCLE,
	RECTANGLE
};

/* // Sauvegarde au format XML :
<sprite2d name="sprite1" source="sprite1.png" speed="">
	<animation name="walk" loop="true">
		<keyframe begin="" end="" xsrc="" ysrc="" widthsrc="" heightsrc=""/>
		<keyframe begin="" end="" xsrc="" ysrc="" widthsrc="" heightsrc=""/>
		...
		<keyframe begin="" end="" xsrc="" ysrc="" widthsrc="" heightsrc=""/>
	</animation>
</sprite2d>
*/
class Animation2d;
class AnimationSet;
class AnimationPool;
class Sprite2d;
class KeyFrame2d;

struct SpriteState
{
    //unsigned int m_iCurrentAnimation;
    string m_sCurrentAnimation;
    Timer * m_pTimer;
	unsigned int m_iCurrentKeyFrame;
	unsigned int m_iBeginningTime;
	bool m_bRunning;
	position2d<int>* m_position;
};

class Hitbox2d
{
	protected:
	Engine *engine;
	enum TypeHitbox m_Type;
	int group;

	public:
	Hitbox2d(Engine *engine, enum TypeHitbox _Type, int _group=0);
	~Hitbox2d();
	virtual bool bCollision(Hitbox2d* hb2d, position2d<int> offset, position2d<int> offsethb2d) const = 0;
	virtual void vDraw(position2d<int> offset) const = 0;
	virtual bool bLoadXML(IrrXMLReader* xml) = 0;
	enum TypeHitbox getType() const;
};

class Hitbox2dCircle : public Hitbox2d
{
	protected:
	position2d<int> m_position;
	int m_Radius;

	public:
	Hitbox2dCircle(Engine *_engine);
	~Hitbox2dCircle();
	bool collision(Hitbox2d& hb2d);
	bool bLoadXML(IrrXMLReader* xml);
	bool bCollision(Hitbox2d* hb2d, position2d<int> offset, position2d<int> offsethb2d) const;
	void vDraw(position2d<int> offset) const;
};


class Hitbox2dRectangle : public Hitbox2d
{
	protected:
	SDL_Rect m_Rect;
	public:
	Hitbox2dRectangle(Engine *_engine);
	~Hitbox2dRectangle();
	bool bCollision(Hitbox2d* hb2d, position2d<int> offset, position2d<int> offsethb2d) const;
	void vDraw(position2d<int> offset) const;
	bool bLoadXML(IrrXMLReader* xml);
};

class KeyFrame2d
{
	protected:
    // DATA
    Engine *engine;
    Animation2d* m_pAnimation2d;
	int m_iDuration;
	int m_iSrc_x1;
	int m_iSrc_y1;
	int m_iSrc_x2;
	int m_iSrc_y2;
	int m_iOffset_x;
	int m_iOffset_y;

	SDL_Rect m_SourceRect;

    int m_iLastFrame;

	Texture *m_texture;
	SDL_Rect m_src; // position dans l'image source

    ThemeFileFinder* file_finder;

	vector<Hitbox2d*> m_HitboxList;
	// STATE

	public:

	KeyFrame2d(Engine *_engine, Animation2d* _pAnimation2d, ThemeFileFinder* file_finder);
	~KeyFrame2d();
	bool bLoadXML(IrrXMLReader* xml);
    void vDraw(position2d<int> dest); // dessine l'image de la keyframe
    void vDrawOpenGL(position2d<int> dest,bool h_flip=false);
    void appliquer(float t[8],bool h_flip=false);
    friend std::ostream &operator << (std::ostream &out, const KeyFrame2d &keyFrame2d);

    // ACCESS
    int iGetDuration();
    int iGetLastFrame();
	bool bCollision(KeyFrame2d* kf2d, position2d<int> offset, position2d<int> offsetkf2d);
};

class Animation2d
{
	protected:
    // DATA
    Engine *engine;
   	int m_iDuration;
	bool m_bLoop;
	std::string m_sName;
	vector<KeyFrame2d*> m_KeyFrame2dList;

    ThemeFileFinder* file_finder;
	// STATE


	public:

	Animation2d(Engine *_engine, ThemeFileFinder* file_finder);
    ~Animation2d();
    bool bLoadXML(IrrXMLReader* xml);
	void vUpdate(SpriteState& state);
    void vDrawOpenGL(const SpriteState& state,bool h_flip=false);
    void appliquer(const SpriteState& state, float t[8]);
    friend std::ostream &operator << (std::ostream &out, const Animation2d &animation2d);
    // ACCESS
    std::string sGetName();
    int iGetDuration();
	KeyFrame2d* pGetKeyFrame(const SpriteState &state);
	bool bLastFrame(const SpriteState &state);
};

class AnimationSet
{
	protected:

    Engine *engine;
    ThemeFileFinder* file_finder;

    map<string,Animation2d*> m_Animation2dList;
    std::string m_sName;

	public:
	AnimationSet(Engine *_engine, ThemeFileFinder* file_finder);
	~AnimationSet();
	bool bLoadXML(const std::string& dossier, const std::string& file);
    bool bSetAnimation(SpriteState& state, const std::string& _animationName);
    void vUpdate(SpriteState& state);
    void vDrawOpenGL(SpriteState& state,bool h_flip=false);
    void appliquer(SpriteState& state, float t[8]);

	// ACCESS
	std::string sGetName();
	KeyFrame2d* pGetKeyFrame(const SpriteState &state);
	bool bLastFrame(const std::string sAnimationName, const SpriteState& state);
};

// Gestionnaire de chargement/recherche d'AnimationSet
class AnimationPool
{
	protected:
	std::string m_cDossier;
	vector<AnimationSet*> animationSetList;
	Engine *engine;
	ThemeFileFinder* file_finder;
	public:

	AnimationPool(Engine *_engine, const std::string &_dossier, ThemeFileFinder* file_finder);
	~AnimationPool();
	AnimationSet* getAnimationSet(const std::string& name);
};

class Sprite2d
{
    protected:
	AnimationSet* m_pAnimationSet;
    Engine *engine;
    ThemeFileFinder* file_finder;

    // STATE
	SpriteState m_state;

    public:

    Sprite2d(Engine *_engine, Timer *_pTimer, ThemeFileFinder* file_finder);
    ~Sprite2d();
    bool init(const std::string& _file, AnimationPool* pAnimationPool);
    bool bSetAnimation(const std::string& _animationName);
    void vStart();
    void vStop();
    virtual void vUpdate();
    void vDrawOpenGL(bool h_flip=false);
    void appliquer(float t[8]);
	void vSetPosition(position2d<int> dest);
    //friend std::ostream &operator << (std::ostream &out, const Sprite2d &sprite2d);
    void vMove(int x, int y);
    // ACCESS
    std::string sGetName();
	KeyFrame2d* pGetKeyFrame();
    bool bCollision(Sprite2d* sprite2d);
    position2d<int> getPosition();
	std::string sGetAnimationName();
	bool bLastFrame(const std::string sAnimationName);
};


#endif
