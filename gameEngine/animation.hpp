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

#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include<vector>
#include <fstream>
/*#include <lib3ds/file.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>*/

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include "algebre.hpp"
#include "glm/glm.hpp"
#include "chemins.h"
#include "theme.h"

#ifndef CODE_AXE_X
#define CODE_AXE_X 1
#endif

#ifndef CODE_AXE_Y
#define CODE_AXE_Y 2
#endif

#ifndef CODE_AXE_Z
#define CODE_AXE_Z 4
#endif

using namespace std;
//using namespace glm;

/**
	Description d'une texture OpenGL.
*/
class Texture
{
	private:

	int id;					//!< Identifiant OpenGL de la texture;
    std::string m_sName;
	/**
		Initialisation de la texture dans OpenGL.
	*/
	void init(SDL_Surface* surface);

	public:

    int w;  ///< Largeur de la texture
    int h;  ///< Hauteur de la texture

	Texture();
	~Texture();

	/**
		Renvoi un pointeur sur l'image de la texture.
	*/
	SDL_Surface* get_surface();

	/**
		Charge l'image contenue dans le fichier "fichier" dans "surface".
	*/
	bool charger(string fichier,ThemeFileFinder* file_finder);
    std::string sGetName();
	/// Applique la texture dans la sc�ne OpenGL.
	void appliquer();

	int getId();
};

// Gestionnaire de chargement/recherche de textures
class TexturePool
{
	protected:
	std::string m_cDossier;
	vector<Texture*> textureList;
	ThemeFileFinder* file_finder;
	public:

	TexturePool(ThemeFileFinder* _file_finder);
	~TexturePool();
	Texture* getTexture(const std::string& name);
};

/**
	Description des propri�t�s mat�rielles d'un objet.
*/
/*class Material
{
    public:
    float ambient[4];	//!< couleur ambiante    {rouge, vert, bleu, alpha}
    float diffuse[4];	//!< couleur diffuse     {rouge, vert, bleu, alpha}
    float specular[4];	//!< couleur sp�culaire  {rouge, vert, bleu, alpha}
    float shininess;	//!< brillance
    char *name;			//!< Nom attribu� au Material
    Texture *texture;	//!< Texture associ�e au Mat�rial
*/
    /**
		Construction d'un Material � partir d'un Lib3dsMaterial.
		La classe Lib3dsMaterial fait partie de la biblioth�que Lib3ds
  		permettant le chargement d'objets 3D � partir de fichiers 3DS.
  		Cette biblioth�que et sa documentation peuvent �tre trouv�es sur le site
  		<a HREF="http://lib3ds.sourceforge.net" target=new>
    	http://lib3ds.sourceforge.net
  		</a>
    */
 /*   Material(Lib3dsMaterial *_material, ThemeFileFinder* file_finder);

    Material(Lib3dsMaterial *_material, TexturePool* texture_pool);
*/
    /**
    	Construction d'un Mat�riel � partir des propri�t�s.
    */
 /*   Material(float _ambient[4],float _diffuse[4],float _specular[4]
				, float _shininess=0, ThemeFileFinder* file_finder=NULL
				,string texture_file=NULL);
*/
    /**
        Constructeur � partir des propri�t� avec r�cup�ration de la texture via TexturePool
    */
/*    Material(float _ambient[4],float _diffuse[4],float _specular[4], float _shininess,string texture_file, TexturePool* texture_pool);
    ~Material();
*/
    /**
		Applique le mat�riel sous openGL.
    */
/*    void apply();
};
*/
/**
	Classe abstraite d�crivant un objet affichable sous openGL.
*/
class Displayable
{
    public:

    /// Affichage de l'objet dans la sc�ne OpenGL.
    virtual void gl_draw()=0;
    /// Affichage des normales de l'objet dans la sc�ne OpenGL.
    virtual void gl_draw_normals()=0;
};

/// polygon compos� de 3 PR3
/*class Facet : public Displayable
{
    public:
    glm::vec3 *point_list[3];
    glm::vec2 *tex_coord_list[3];
    bool tex_coord;
    Material *material;
    glm::vec3 normal;
    glm::vec3 *normal_list[3]; // liste des normales aux sommets
    Facet(Lib3dsFace _face, glm::vec3 *all_points, glm::vec2 *all_tex_coord
    			,vector<Material *> *material_list);
    ~Facet();
    void gl_draw();
    void gl_draw_normals();
};
*/
/// Liste de Facet
/*class Mesh : public Displayable
{
    public:
    Facet ** facet_list;
    Facet** ptr;
    int facet_list_size;
    glm::vec3 * point_list;
    int point_list_size;
    glm::vec3 * normal_list;
    Material *material;
    glm::vec2 * tex_coord_list;

    Mesh(Lib3dsMesh * _3ds_mesh, vector<Material *> *material_list);
    ~Mesh();
    void CreatePointNormals();
    void gl_draw();
    void gl_draw_normals();
};
*/
/// Liste de Mesh
/*class Object3d : public Displayable
{
    public:
    vector<Mesh *> mesh_list;
    vector<Material *>material_list;

    Object3d();
    Object3d(string file_name, ThemeFileFinder* file_finder);
    Object3d(string file_name, ThemeFileFinder* file_finder, TexturePool* texture_pool);
    ~Object3d();
    void get_min_and_max(glm::vec3& min, glm::vec3& max);
    glm::vec3 get_size();
    glm::vec3 get_center();
    glm::vec3 get_center(glm::vec3& min, glm::vec3& max);
    void resize(float size,int axe_code); // axe_code = CODE_AXE_X | CODE_AXE_Y | CODE_AXE_Z
    void center(glm::vec3 position_centre=glm::vec3(0.0,0.0,0.0));
    void gl_draw();
    void gl_draw_normals();
};
*/
/**
	Transformation g�n�rique.
*/
class  TransfoAnim
{
	protected:

	float date_debut;	//!< Date de d�but de la transformation
	float date_fin;		//!< Date de fin de la transformation
	float valeur;		//!< Valeur de la transformation

	public:

	TransfoAnim(float _valeur, float _date_debut,float _date_fin);

	/**
		Applique la transformation en fonction de la date donn�e en
		param�tre, par interpolation.
	*/
	virtual void appliquer(float date)=0;
	bool finished(float date);
};

/**
	Translation sur l'axe X.
*/
class TranslationX_Anim : public TransfoAnim
{
	public:

	TranslationX_Anim(float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Translation sur l'axe Y.
*/
class TranslationY_Anim : public TransfoAnim
{
	public:

	TranslationY_Anim(float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Translation sur l'axe Z.
*/
class TranslationZ_Anim : public TransfoAnim
{
	public:

	TranslationZ_Anim(float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Changement d'�chelle sur X.
*/
class EchelleX_Anim : public TransfoAnim
{
	public:

	EchelleX_Anim(float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Changement d'�chelle sur Y.
*/
class EchelleY_Anim : public TransfoAnim
{
	public:

	EchelleY_Anim(float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Changement d'�chelle sur Z.
*/
class EchelleZ_Anim : public TransfoAnim
{
	public:

	EchelleZ_Anim(float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Rotation d'un angle "valeur" (en degr�s) autour de l'axe (x,y,z).
*/
class Rotation_Anim : public TransfoAnim
{
	protected:
	float x;
	float y;
	float z;

	public:

	Rotation_Anim(float _x, float _y, float _z
 					, float _valeur, float _date_debut,float _date_fin);
	void appliquer(float date);
};

/**
	Liste des transformations constituant une animation.
*/
class Liste_TransfoAnim
{
	public:
	TransfoAnim ** liste;	//!< Liste des transformations de l'animation
	int nb_transfo;			//!< Nombre de transformations de la liste
	int max_transfo;		//!< Nombre maximum de transformations dans la liste
	int duree;				//!< Dur�e totale de l'animation

	/**
		Construction d'une animation � partir d'un fichier .ani
	*/
	Liste_TransfoAnim(string fichier, ThemeFileFinder* file_finder);
	/**
		Construction d'une animation vide
	*/
	Liste_TransfoAnim(int _max_transfo, int _duree);
	~Liste_TransfoAnim();
	/**
		Applique les transformations en cours � la date donn�e en param�tre.
	*/
	void appliquer(float date);
    bool finished(float date);
	protected:

	/**
		Lecture d'une ligne correspondant � une translation dans le fichier.
	*/
	void lire_translation(ifstream *f,int rang);

	/**
		Lecture d'une ligne correspondant � un changement d'�chelle
		dans le fichier.
	*/
	void lire_echelle(ifstream *f,int rang);

	/**
		Lecture d'une ligne correspondant � une rotation dans le fichier.
	*/
	void lire_rotation(ifstream *f,int rang);

	public:

	/**
		Ajout d'une translation
	*/
	bool add_translation(char type_transfo,float debut,float fin,float valeur);

	/**
		Ajout d'un changement d'�chelle
	*/
	bool add_echelle(char type_transfo,float debut,float fin,float valeur);

	/**
		Ajout d'une rotation
	*/
	bool add_rotation(float debut, float fin, float valeur, float x, float y,float z);
};

/**
	Une Animation contient une Liste_TransfoAnim (donc une liste des
	transformations � effectuer pour r�aliser l'animation. Elle poss�de des
	fonctions permettant de contr�ler l'animation dont elle a la charge.
	Il s'agit donc d'une sorte de "lecteur d'animation".
*/
class Animation
{
	protected:

	Liste_TransfoAnim * liste_transfo;	//!< Liste des transformations.
	int date_debut;						//!< Date de d�but de l'animation
	bool marche;						//!< Marche/Arr�t
	int sens;							//!< 0 -> sens normal. 1-> sens inverse
	bool pause_mode;					//!< Pause/unPause
	int date_pause;						/**!< Date � laquelle l'animation a �t�
										mise en mode Pause */

	public:

	Animation(Liste_TransfoAnim * _liste_transfo=NULL);

	~Animation();

	/// Lancer l'animation.
	void start(int date);

	/// Stopper l'animation.
	void stop();

	/// Mise en pause de l'animation.
	void pause(int date);

	/// Reprendre l'animation mise en pause.
	void unpause(int date);

	/// Change le sens de parcour de l'animation.
	void inverse();

	/**
		Appliquer les transformations correspondant � l'animation � la date
		donn�e.
	*/
	void appliquer(int date);

	/// Renvoi la dur�e totale de l'animation.
	int get_duree();
};

/**
	Cette classe regroupe un ensemble d'animations et permet de choisir une
	animation active parmi celles-ci. Les fonctions de contr�les permettent
	ensuite de contr�ler l'animation courante.
*/
class Liste_Animations
{
	protected:

	Animation **liste;				//!< Liste d'animations.
	unsigned int nb_animations;		//!< Nombre d'animations dans la liste.
	unsigned int nb_max_animations;	//!< Taille maximum de la liste.
	int animation_courante;			//!< Rang de l'animation courante.

	public:

	Liste_Animations(unsigned int _nb_max_animations);
	~Liste_Animations();

	/// Ajouter une animation dans la liste.
	bool add(Liste_TransfoAnim * _liste_transfo);

	/// Change l'animation courante.
	bool set(unsigned int rang_animation);

	/// Lancer l'animation courante.
	void start(int date);

	/// Stopper l'animation courante.
	void stop();

	/// Mettre l'animation courante en pause.
	void pause(int date);

	/// Repprendre l'animation mise en pause.
	void unpause(int date);

	/// Inverse le sens de l'animation courante.
	void inverse();

	/// Appliquer les transformations de l'animation courante.
	void appliquer(int date);

	/// Renvoi la dur�e totale de l'animation courante.
	int get_duree();
};

#endif
