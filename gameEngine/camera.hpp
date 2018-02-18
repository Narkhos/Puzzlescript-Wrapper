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

#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef EPSILON
#define EPSILON   1E-5
#endif

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/epsilon.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "display.hpp"
#include "gltext2d.hpp"

/**
	Description d'une caméra appliquable dans une scène OpenGL.
*/
class Camera
{
	public:

	glm::mat4 M; // matrice opengl. Elle est construite à partir de zoom et
 				 //rotation. Pour ne pas avoir à la recalculer à chaque
 				 // affichage, mais uniquement à chaque modification de la
 				 // position de la Camera

	private:
	glm::dvec3 position; 	// Position courante de la Camera
	glm::dvec3 position_initiale; 	// Au cours d'une interpolation de mouvement:position de début du mouvement
	glm::dvec3 position_finale; 	// Au cours d'une interpolation de mouvement:position de fin du mouvement

    Display * display;

	bool interpol_pos; // est à vrai quand une interpolation de position est en cours.
	int date_debut_pos;		// date de début de l'interpolation de position courante.
	int date_fin_pos;		// date de fin de l'interpolation de position courante.

	double vitesse_translation;

    glm::dquat orientation;	// Orientation courante de la Camera
	glm::dquat orientation_initiale;	// Au cours d'une interpolation de mouvement:orientation de début
	glm::dquat orientation_finale;	// Au cours d'une interpolation de mouvement:orientation de fin

	bool interpol_orient; // est à vrai quand une interpolation d'orientation est en cours.
	int date_debut_orient;		// date de début de l'interpolation d'orientation courante.
	int date_fin_orient;		// date de fin de l'interpolation d'orientation courante.

	double vitesse_angulaire;

	public:

	/**
		La vitesse angulaire est donnée en degrés/seconde.
		Elle est converti en rad/milliseconde.
		La vitesse de translation est donnée en unités/seconde.
		Elle est convertie en unités/milliseconde
	*/
	Camera(Display * _display,double _vitesse_translation=10.0,double _vitesse_angulaire=10.0
		,float angle_x=0.0, float angle_y=0.0,float angle_z=0.0
			,float x=0.0,float y=0.0,float z=-8.0)
			:display(_display),position(x,y,z)
	{
		vitesse_angulaire=((_vitesse_angulaire/180.0)*3.14)/1000;

		interpol_orient=false;

		vitesse_translation=_vitesse_translation/1000;
		interpol_pos=false;

		//orientation = from_angles(angle_x,angle_y,angle_z);
		orientation = glm::dquat(glm::dvec3((angle_x/180.0*glm::pi<float>())/2.0, (angle_y/180.0*glm::pi<float>())/2.0, (angle_z/180.0*glm::pi<float>())/2.0));
		//orientation = glm::quat(glm::vec3(angle_x, angle_y, angle_z));
		orientation = glm::normalize(orientation);

		actualiser_M();
	}

	private:

	/**
		Génère la matrice M, utilisable par OpenGL, en fonction des
		transformations (translation et rotation) exprimés par les
		membres orientation et position.
	*/
	void actualiser_M()
	{
		glm::dmat4 rot = glm::toMat4(orientation);
		glm::dmat4 trans = glm::translate(glm::dmat4(1.0f), position);
		//M = trans;
        M = trans*rot;
	}

	public:

	/**
		Lance l'interpolation de rotation de la caméra, de son orientation
		actuelle à celle donnée en paramètre.
	*/
	void changer_orientation(int date,glm::vec3 axe,float angle)
	{
		orientation_finale=glm::dquat(cos((angle/180.0*3.14)/2.0)
  					,sin((angle/180.0*3.14)/2.0)*axe.x
					,sin((angle/180.0*3.14)/2.0)*axe.y
					,sin((angle/180.0*3.14)/2.0)*axe.z);

		// Si l'orientation est déjà l'orientation finale, on ne fait pas
		// d'interpolation
//		if (glm::epsilonEqual<glm::quat>(orientation,orientation_finale,glm::quat())) return;

		orientation_initiale=glm::normalize(orientation);
		orientation_finale = glm::normalize(orientation_finale);
		interpol_orient=true;
		date_debut_orient=date;

		// Calcul de la durée de la rotation en fonction de la vitesse angulaire
		glm::dquat tmp=orientation_finale+(-orientation_initiale);
		tmp = glm::normalize(tmp);
		//int duree=(int)abs(tmp.w/vitesse_angulaire);
        int duree = 800;
		date_fin_orient=date_debut_orient+duree;
	}

	void changer_orientation(glm::vec3 axe,float angle)
	{
		orientation_finale=glm::dquat(cos((angle/180.0*3.14)/2.0)
  					,sin((angle/180.0*3.14)/2.0)*axe.x
					,sin((angle/180.0*3.14)/2.0)*axe.y
					,sin((angle/180.0*3.14)/2.0)*axe.z);

		// Si l'orientation est déjà l'orientation finale, on ne fait pas
		// d'interpolation
//        if(glm::epsilonEqual<glm::quat>(orientation,orientation_finale,glm::quat)) return;
		orientation_finale = glm::normalize(orientation_finale);
		orientation = orientation_finale;
		interpol_orient=false;
        this->actualiser_M();
	}

	/**
		Lance l'interpolation de rotation de la caméra, de son orientation
		actuelle à celle donnée en paramètre.
	*/
	void changer_orientation(int date,glm::dquat final)
	{
		orientation_finale=final;

		// Si l'orientation est déjà l'orientation finale, on ne fait pas
		// d'interpolation
        if(glm::epsilonEqual<glm::dquat>(orientation,orientation_finale,glm::dquat())) return;

		orientation_initiale=orientation;

		orientation_finale = glm::normalize(orientation_finale);
		interpol_orient=true;
		date_debut_orient=date;

		// Calcul de la durée de la rotation en fonction de la vitesse angulaire
		glm::dquat tmp=orientation_finale+(-orientation_initiale);
		int duree=(int)abs(tmp.w/vitesse_angulaire);

		date_fin_orient=date_debut_orient+duree;
	}

	/**
		Lance l'interpolation de translation de la caméra, de sa position
		actuelle à celle donnée en paramètre.
	*/
	void changer_position(int date,glm::dvec3 pos)
	{
		position_finale=pos;
		if(position==position_finale) return;

		position_initiale=position;

		interpol_pos=true;
		date_debut_pos=date;
		int duree=(int)(abs(glm::length(pos-position))/(vitesse_translation));
		date_fin_pos=date_debut_pos+duree;
	}

    void deplacer(glm::dvec3 dep)
	{
		position=position+dep;
		this->actualiser_M();
	}

	/**
		Renvoi TRUE si la caméra est en mouvement à l'appel de la fonction.
	*/
	bool mouvement_en_cours()
	{
		// Si une interpolation d'orientation ou de position est en cours
		// on renvoi true.
		if(interpol_orient || interpol_pos) return true;

		return false;
	}

    void rotationSouris(int delta_x, int delta_y, float sensibilite, bool z=false)
    {
        glm::dquat q;
        if(z)
        {
            //q.from_angles(0.0,(float)delta_x*sensibilite,-(float)delta_y*sensibilite);
            //q = from_angles(0.0,0.0,-(float)delta_y*sensibilite);
            q = glm::dquat(glm::dvec3(0.0,0.0,-(float)delta_y*sensibilite));
        }
        else
        {
            //q = from_angles((float)delta_y*sensibilite,(float)delta_x*sensibilite,0.0);
             q = glm::dquat(glm::dvec3((float)delta_y*sensibilite,(float)delta_x*sensibilite,0.0));
        }
        q = this->orientation*q;
        q = glm::normalize(q);
        this->orientation = q;
        this->actualiser_M();
    }

    void rotationSouris(glm::dvec3 touchingStart, glm::dvec3 touchingEnd, float sensibilite=1.0)
    {
        if(touchingStart==touchingEnd) return; // si la souris n'a pas bougé, on ne fait rien (sinon on obtient des résultats inatendu et on se retrouve avec un écran noir)
        glm::dvec3 axis = glm::cross(touchingStart,touchingEnd);
        axis = glm::normalize(axis);
        float dot = glm::dot(touchingStart,touchingEnd);
        float angle = acosf(dot);
        this->rotationAngle(axis,angle*sensibilite);
    }

    void rotationSouris(glm::dvec3 touchingStart, glm::dvec3 touchingEnd, glm::dvec3 axis, float sensibilite=1.0, bool sens = true)
    {
        if(touchingStart==touchingEnd) return; // si la souris n'a pas bougé, on ne fait rien (sinon on obtient des résultats inatendu et on se retrouve avec un écran noir)
        float dot = glm::dot(touchingStart,touchingEnd);
        float angle = acosf(dot);
        double distx = touchingStart.x - touchingEnd.x;
        if(distx > 0)
        {
            angle = - angle;
        }


        this->rotationAngle(axis,angle*sensibilite, sens);
    }

    void rotationAngle(glm::dvec3 axe,float angle, bool sens = true)
    {
        glm::dquat tmp = glm::dquat(cos((angle/180.0*3.14)/2.0)
  					,sin((angle/180.0*3.14)/2.0)*axe.x
					,sin((angle/180.0*3.14)/2.0)*axe.y
					,sin((angle/180.0*3.14)/2.0)*axe.z);

		tmp = glm::normalize(tmp);
		if(sens)
        {
            orientation = tmp*orientation;
        }
        else
        {
            orientation = orientation*tmp;
        }
		orientation = glm::normalize(orientation);
		interpol_orient=false;
        this->actualiser_M();
    }

	/**
		Applique les transformations de la caméra à la scène OpenGL.
		La date permet à la caméra de calculer les interpolations lorsqu'un
		mouvement est en cours.
	*/
	void appliquer(int date)
	{
		// Interpolation d'orientation
		if(interpol_orient)
		{
			double t;
			t=((double)(date-date_debut_orient))/((double)(date_fin_orient-date_debut_orient));

			if(t>1.0)
			{
				t=1.0;
				interpol_orient=false;
			}
			orientation = glm::slerp(orientation_initiale,orientation_finale,t);
			orientation = glm::normalize(orientation);
			actualiser_M();
		}

		// Interpolation de position
		if(interpol_pos)
		{
			double t;
			t=((double)(date-date_debut_pos))/((double)(date_fin_pos-date_debut_pos));
			if(t>1.0)
			{
				t=1.0;
				interpol_pos=false;
			}
			position.x=position_initiale.x+(position_finale.x-position_initiale.x)*t;
			position.y=position_initiale.y+(position_finale.y-position_initiale.y)*t;
			position.z=position_initiale.z+(position_finale.z-position_initiale.z)*t;

			actualiser_M();
		}

		// Application de la matrice de transformation à OpenGL
		glMultMatrixf(&M[0][0]);
	}

    glm::dvec3 projectOntoSurface(glm::dvec3 touchPoint)
    {
        // rayon de l'Arcball
        float radius = display->width/3;
        // Centre de l'Arcball
        glm::dvec3 center = glm::dvec3(display->width/2, display->height/2, 0);
        cout << "touchPoint : " << glm::to_string(touchPoint) << endl;
        glm::dvec3 P = touchPoint - center;
        cout << "P : " << glm::to_string(P) << endl;
        // Flip the y-axis because pixel coords increase toward the bottom.
        P = glm::dvec3(P.x, P.y * -1, P.z);

        double radius2 = radius * radius;
        double length2 = P.x*P.x + P.y*P.y;

        if (length2 <= radius2)
            P.z = std::sqrt(radius2 - length2);
        else
        {
            P.x *= radius / std::sqrt(length2);
            P.y *= radius / std::sqrt(length2);
            P.z = 0;
            /*P.z = radius2 / (2.0 * sqrt(length2));
            double length = sqrt(length2 + P.z * P.z);
            P = P/length;*/
        }
        P = glm::normalize(P);
        cout << "P Normalized : " << glm::to_string(P) << endl;
        return P;
    }

    glm::vec2 convertToScreenCoord(glm::dvec3 pos)
    {
        glm::dvec4 res = (glm::dvec4)(display->M*M*glm::vec4(pos,1));
        res /= res.w;

        res.x = display->width*(res.x + 1.0)/2.0;
        res.y = display->height * (1.0 - ((res.y + 1.0) / 2.0));

        return glm::vec2((float)res.x, (float)res.y);
    }

    glm::dvec3 convertToWorldCoord(glm::dvec3 pos)
    {
        // pos = Vector3::Transform(pos, viewMatrix);
        glm::dvec4 res = (glm::dvec4)(glm::transpose(M)*glm::vec4(pos,1));
        res /= res.w;

        //cout << "convertToWorldCoord : " << glm::to_string(pos) << " ==> " << glm::to_string(res) << endl;

        return glm::dvec3(res);
    }

    void drawVec(glm::dvec3 v,float r=1.0, float g=1.0, float b=1.0, float w=1.0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glLineWidth(w);
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        // axe x
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(v.x, v.y, v.z);
        glEnd();
        glBegin(GL_POINTS);
        glVertex3f(v.x, v.y, v.z);
        glEnd();
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }

    void drawVec(glm::dvec3 v1, glm::dvec3 v2,float r=1.0, float g=1.0, float b=1.0, float w=1.0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glLineWidth(w);
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glEnd();
        glBegin(GL_POINTS);
        glVertex3f(v2.x, v2.y, v2.z);
        glEnd();
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }

    void afficherRepere(glText2d *texte=NULL)
    {

        glLineWidth(1.0);
        glPointSize(4.0);

        // axe x
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glEnd();
        glBegin(GL_POINTS);
        glVertex3f(1.0, 0.0, 0.0);
        glEnd();

        if(texte != NULL)
        {
            float color[3] = {1.0, 0.0, 0.0};
            glm::vec2 pos_txt = convertToScreenCoord(glm::dvec3(1.0, 0.0, 0.0));
            texte->display("x",pos_txt.x,pos_txt.y, color);
            cout << "Pos texte x : " << glm::to_string(pos_txt)<<endl;
        }

        // axe y
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
        glEnd();
        glBegin(GL_POINTS);
        glVertex3f(0.0, 1.0, 0.0);
        glEnd();

        if(texte != NULL)
        {
            float color[3] = {0.0, 1.0, 0.0};
            glm::vec2 pos_txt = convertToScreenCoord(glm::dvec3(0.0, 1.0, 0.0));
            texte->display("y",pos_txt.x,pos_txt.y, color);
            cout << "Pos texte y : " << glm::to_string(pos_txt)<<endl;
        }
        // axe z
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);
        glEnd();
        glBegin(GL_POINTS);
        glVertex3f(0.0, 0.0, 1.0);
        glEnd();

        if(texte != NULL)
        {
            float color[3] = {0.0, 0.0, 1.0};
            glm::vec2 pos_txt = convertToScreenCoord(glm::dvec3(0.0, 0.0, 1.0));
            texte->display("z",pos_txt.x,pos_txt.y, color);
            cout << "Pos texte z : " << glm::to_string(pos_txt)<<endl;
        }

        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }
};

#endif
