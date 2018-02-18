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

#include <istream>
#include <stdlib.h>
#include "animation.hpp"

////////////////////////////////////////////////////////////////////////////////
// Classe Texture

Texture::Texture():id(-1)
{}

Texture::~Texture()
{
	glDeleteTextures(1,(GLuint*)&(this->id));
}

void Texture::init(SDL_Surface* surface)
{
	// Si la texture n'a pas encore d'identifiant sous OpenGL,
	// on lui en attribut un.
	if(this->id==-1)
	{
		glGenTextures(1,(GLuint*)&(this->id));
	}

	glBindTexture(GL_TEXTURE_2D,(GLuint)(this->id));

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);      //we repeat the pixels in the edge of the texture, it will hide that 1px wide line at the edge of the cube, which you have seen in the video
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);      //we do it for vertically and horizontally (previous line)

    if(surface->format->format == SDL_PIXELFORMAT_ABGR8888)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA, surface->w, surface->h,  0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8, surface->w, surface->h,0,GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
    }



}

bool Texture::charger(string fichier,ThemeFileFinder* file_finder)
{
	string chemin_complet;
    this->m_sName = std::string(fichier);
	chemin_complet=file_finder->find(REP_TEXTURES,fichier);

	// Si une image était déjà stockée, on la supprime.
	if(this->id!=-1)
	{
		glDeleteTextures(1,(GLuint*)&(this->id));
		this->id=-1;
	}

	// On charge l'image à partir du fichier.
	SDL_Surface* surface=IMG_Load_RW(SDL_RWFromFile(chemin_complet.c_str(), "rb"), 1);

	if(!surface)
 	{
 	    #ifdef __DEBUG__
 	    printf("IMG_Load_RW: %s\n", IMG_GetError());
 	    #endif
 		return false;

	}

    this->w = surface->w;
    this->h = surface->h;

	// Insertion de la texture dans OpenGL.
	this->init(surface);

    SDL_FreeSurface(surface);
    surface=NULL;

	return true;
}

std::string Texture::sGetName()
{
    return m_sName;
}

void Texture::appliquer()
{
   	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D,(GLuint)(this->id));

	return;
}

int Texture::getId()
{
    return this->id;
}

/********** TEXTUREPOOL ************/

TexturePool::TexturePool(ThemeFileFinder* _file_finder)
	:file_finder(_file_finder)
{
}

TexturePool::~TexturePool()
{
	for(int i = 0; i < textureList.size(); i++)
	{
		delete textureList[i];
		textureList[i] = NULL;
	}
}

Texture* TexturePool::getTexture(const std::string& name)
{
	for(int i = 0; i < textureList.size(); ++i)
	{
		if(textureList[i]->sGetName().compare(name) == 0)
		{
			// La texture à été trouvée
			return textureList[i];
		}
	}

	// La texture n'a pas été trouvée, on tente de le charger
	Texture* tmp;
	tmp = new Texture();

	if(tmp->charger(name.c_str(),file_finder))
	{
		textureList.push_back(tmp);
		return tmp;
	}
	else
	{
		delete tmp;
		tmp = NULL;
		return NULL;
	}

}


////////////////////////////////////////////////////////////////////////////////
// Classe Material

/*Material::Material(Lib3dsMaterial *_material, ThemeFileFinder* file_finder)
{
	this->name=NULL;

    if (_material==NULL)
    {
        for(int i=0;i<4;i++)
        {
                ambient[i]=0;
                diffuse[i]=1.0;
                specular[i]=0.1;
        }
        shininess=0;

        return;
    }

    this->name=new char[strlen(_material->name)+1];
    strcpy(name,_material->name);

    for(int i=0;i<4;i++)
    {
        ambient[i]=_material->ambient[i];
        diffuse[i]=_material->diffuse[i];
        specular[i]=_material->specular[i];
    }

    shininess=pow(2,10.0*_material->shininess);
    if (shininess>128.0)
    {
              shininess=128.0;
    }

    if(strlen(_material->texture1_map.name)==0)
    {
    	texture=NULL;
	}
    else
    {
    	texture=new Texture();

    	if(!texture->charger(_material->texture1_map.name,file_finder))
    	{
    		delete texture;
    		texture=NULL;
    	}
    }
}

Material::Material(Lib3dsMaterial *_material, TexturePool* texture_pool)
{
	this->name=NULL;

    if (_material==NULL)
    {
        for(int i=0;i<4;i++)
        {
                ambient[i]=0;
                diffuse[i]=1.0;
                specular[i]=0.1;
        }
        shininess=0;

        return;
    }

    this->name=new char[strlen(_material->name)+1];
    strcpy(name,_material->name);

    for(int i=0;i<4;i++)
    {
        ambient[i]=_material->ambient[i];
        diffuse[i]=_material->diffuse[i];
        specular[i]=_material->specular[i];
    }

    shininess=pow(2,10.0*_material->shininess);
    if (shininess>128.0)
    {
              shininess=128.0;
    }

    if(strlen(_material->texture1_map.name)==0)
    {
    	texture=NULL;
	}
    else
    {
    	texture=texture_pool->getTexture(_material->texture1_map.name);
    }
}

Material::Material(float _ambient[4],float _diffuse[4],float _specular[4]
				, float _shininess, ThemeFileFinder* file_finder
				,string texture_file)
{
	for(int i=0;i<4;i++)
	{
		ambient[i]=_ambient[i];
		diffuse[i]=_diffuse[i];
		specular[i]=_specular[i];
	}

	shininess=_shininess;

	if(texture_file.compare("")!=0)
	{
		this->texture=new Texture();
		this->texture->charger(texture_file, file_finder);
	}
	else
	{
		texture=NULL;
	}
}

Material::Material(float _ambient[4],float _diffuse[4],float _specular[4], float _shininess,string texture_file, TexturePool* texture_pool)
{
	for(int i=0;i<4;i++)
	{
		ambient[i]=_ambient[i];
		diffuse[i]=_diffuse[i];
		specular[i]=_specular[i];
	}

	shininess=_shininess;

	if(texture_file.compare("")!=0 && texture_pool)
	{
		this->texture=texture_pool->getTexture(texture_file);
	}
	else
	{
		texture=NULL;
	}
}


Material::~Material()
{
	delete [] name;
	name=NULL;
	delete texture;
}

void Material::apply()
{
            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
            glMaterialf(GL_FRONT, GL_SHININESS, shininess);
            if(texture)
            {
            	texture->appliquer();
        	}
        	else
        	{
        		glDisable(GL_TEXTURE_2D);
        	}
}
*/
////////////////////////////////////////////////////////////////////////////////
// Classe Facet : public Displayable

/*Facet::Facet(Lib3dsFace _face, glm::vec3 *all_points, glm::vec2 *all_tex_coord
				, vector<Material *> *material_list)
{
	material=NULL;
	for(int i=0;i<material_list->size();i++)
	{
		if(strcmp(_face.material,(*material_list)[i]->name)==0)
		{
			material=(*material_list)[i];
			break;
		}
	}

	if(all_tex_coord)
	{
		tex_coord=true;
		tex_coord_list[0]=&(all_tex_coord[_face.points[0]]);
		tex_coord_list[1]=&(all_tex_coord[_face.points[1]]);
		tex_coord_list[2]=&(all_tex_coord[_face.points[2]]);
	}
	else
	{
		tex_coord=false;
	}

    point_list[0]=&(all_points[_face.points[0]]);
    point_list[1]=&(all_points[_face.points[1]]);
    point_list[2]=&(all_points[_face.points[2]]);
    normal=glm::vec3(_face.normal[0],_face.normal[1],_face.normal[2]);
}

Facet::~Facet()
{}

void Facet::gl_draw()
{
*/
    /*
    if(material)
    {
    	material->apply();
	}*/
/*
    glBegin(GL_POLYGON);
    if(tex_coord)
    {
	    	glTexCoord2f(tex_coord_list[0]->x,tex_coord_list[0]->y);
    		glNormal3f(normal_list[0]->x, normal_list[0]->y,normal_list[0]->z);
	        glVertex3f(point_list[0]->x,point_list[0]->y,point_list[0]->z);

	    	glTexCoord2f(tex_coord_list[1]->x,tex_coord_list[1]->y);
    		glNormal3f(normal_list[1]->x, normal_list[1]->y,normal_list[1]->z);
	        glVertex3f(point_list[1]->x,point_list[1]->y,point_list[1]->z);

	    	glTexCoord2f(tex_coord_list[2]->x,tex_coord_list[2]->y);
    		glNormal3f(normal_list[2]->x, normal_list[2]->y,normal_list[2]->z);
	        glVertex3f(point_list[2]->x,point_list[2]->y,point_list[2]->z);
    }
    else
    {
    		glNormal3f(normal_list[0]->x, normal_list[0]->y,normal_list[0]->z);
	        glVertex3f(point_list[0]->x,point_list[0]->y,point_list[0]->z);

    		glNormal3f(normal_list[1]->x, normal_list[1]->y,normal_list[1]->z);
	        glVertex3f(point_list[1]->x,point_list[1]->y,point_list[1]->z);

    		glNormal3f(normal_list[2]->x, normal_list[2]->y,normal_list[2]->z);
	        glVertex3f(point_list[2]->x,point_list[2]->y,point_list[2]->z);
    }
    glEnd();
}

void Facet::gl_draw_normals()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		for(int i=0;i<3;i++)
		{
			glVertex3f(point_list[i]->x,point_list[i]->y,point_list[i]->z);

			glVertex3f(point_list[i]->x+normal_list[i]->x
						,point_list[i]->y+normal_list[i]->y
						,point_list[i]->z+normal_list[i]->z);
		}
	glEnd();
}
*/
////////////////////////////////////////////////////////////////////////////////
// class Mesh : public Displayable

/*Mesh::Mesh(Lib3dsMesh * _3ds_mesh, vector<Material *> *material_list)
{
	tex_coord_list=NULL;
	material=NULL;

	if(_3ds_mesh->texels>0)
    {
    	tex_coord_list=new glm::vec2[_3ds_mesh->texels];
    	for(int i=0;i<_3ds_mesh->texels;i++)
	    {
	    	tex_coord_list[i].x=_3ds_mesh->texelL[i][0];
	    	tex_coord_list[i].y=_3ds_mesh->texelL[i][1];
    	}
	}

	for(int i=0;i<material_list->size();i++)
	{
		if(strcmp(_3ds_mesh->faceL[0].material,(*material_list)[i]->name)==0)
		{
			material=(*material_list)[i];
			break;
		}
	}


    point_list_size=_3ds_mesh->points;
    point_list=new glm::vec3[point_list_size];
    for(int i=0;i<point_list_size;i++)
    {
        point_list[i]=glm::vec3(_3ds_mesh->pointL[i].pos[0],_3ds_mesh->pointL[i].pos[1],_3ds_mesh->pointL[i].pos[2]);
    }

    facet_list_size=_3ds_mesh->faces;
    facet_list=new Facet*[facet_list_size];
    for(int i=0;i<facet_list_size;i++)
    {
        facet_list[i]=new Facet(_3ds_mesh->faceL[i],point_list, tex_coord_list,material_list);
    }
    normal_list=NULL;
    CreatePointNormals();
}

Mesh::~Mesh()
{
	for(int i=0;i<facet_list_size;i++)
	{
		delete facet_list[i];
		facet_list[i]=NULL;
	}

	delete [] facet_list;
	facet_list=NULL;

	delete [] point_list;
	point_list=NULL;

	if(tex_coord_list)
	{
		delete [] tex_coord_list;
	}

	delete [] normal_list;
	normal_list=NULL;
}

void Mesh::CreatePointNormals()
{
	// Initialisation de la liste de normales aux points
	if(normal_list)
	{
		delete [] normal_list;
		normal_list=NULL;
	}
	normal_list=new glm::vec3[point_list_size];

	for(int i=0;i<point_list_size;i++)
	{
		normal_list[i].x=0;
		normal_list[i].y=0;
		normal_list[i].z=0;
	}

	// Calcul des Normales aux points
	glm::vec3 *point_list_ptr=point_list;

	for(int i=0;i<facet_list_size;i++)
	{
		glm::vec3 *normale[3];
		for(int j=0;j<3;j++)
		{
			int rang=(facet_list[i]->point_list[j] - point_list_ptr);

			normale[j]=&(normal_list[rang]);

			if((normale[j]->x==0) && (normale[j]->y==0) && (normale[j]->z==0))
			{
				normale[j]->x=facet_list[i]->normal.x;
				normale[j]->y=facet_list[i]->normal.y;
				normale[j]->z=facet_list[i]->normal.z;
			}
			else
			{
				(*(normale[j]))+=facet_list[i]->normal;
			}
			*(normale[j]) = glm::normalize(*(normale[j]));
		}

		for(int j=0;j<3;j++)
		{
			facet_list[i]->normal_list[j]=normale[j];
		}
	}
}

void Mesh::gl_draw()
{
    if(material)
    {
    	material->apply();
	}
    ptr=facet_list;
    for(int i=facet_list_size;i--;)
    {
        (*ptr)->gl_draw();
        ptr++;
    }
}

void Mesh::gl_draw_normals()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		for(int i=0;i<point_list_size;i++)
		{
			glVertex3f(point_list[i].x,point_list[i].y,point_list[i].z);

			glVertex3f(point_list[i].x+normal_list[i].x
						,point_list[i].y+normal_list[i].y
						,point_list[i].z+normal_list[i].z);
		}
	glEnd();
}

////////////////////////////////////////////////////////////////////////////////
// class Object3d

Object3d::Object3d()
{

}

Object3d::Object3d(string file_name, ThemeFileFinder* file_finder)
{
	string chemin_complet;
	chemin_complet=file_finder->find(REP_MESHS,file_name);

	Lib3dsFile *f=0;

	f=lib3ds_file_load(chemin_complet.c_str());
	if (!f)
	{
		fprintf(stderr, "***ERROR***\nLoading file failed\n");
		exit(1);
	}

	// Boucle sur tous les noeuds du fichier
	Lib3dsMesh * mesh;
	Lib3dsMaterial *_material;

  	for (_material=f->materials; _material!=0; _material=_material->next)
    {
    	material_list.push_back(new Material(_material,file_finder));
    }

    for (mesh=f->meshes; mesh!=0; mesh=mesh->next)
    {
    	mesh_list.push_back(new Mesh(mesh,&material_list));
    }

    lib3ds_file_free(f);
}

Object3d::Object3d(string file_name, ThemeFileFinder* file_finder, TexturePool* texture_pool)
{
	string chemin_complet;
	chemin_complet=file_finder->find(REP_MESHS,file_name);

	Lib3dsFile *f=0;

	f=lib3ds_file_load(chemin_complet.c_str());
	if (!f)
	{
		fprintf(stderr, "***ERROR***\nLoading file failed\n");
		exit(1);
	}

	// Boucle sur tous les noeuds du fichier
	Lib3dsMesh * mesh;
	Lib3dsMaterial *_material;

  	for (_material=f->materials; _material!=0; _material=_material->next)
    {
    	material_list.push_back(new Material(_material,texture_pool));
    }

    for (mesh=f->meshes; mesh!=0; mesh=mesh->next)
    {
    	mesh_list.push_back(new Mesh(mesh,&material_list));
    }

    lib3ds_file_free(f);
}

Object3d::~Object3d()
{
	for(int i=0;i<mesh_list.size();i++)
	{
		delete mesh_list[i];
		mesh_list[i]=NULL;
	}

	for(int i=0;i<material_list.size();i++)
	{
		delete material_list[i];
		material_list[i]=NULL;
	}
}

void Object3d::get_min_and_max(glm::vec3& min, glm::vec3& max)
{
	if(mesh_list.size()==0)
 	{
 		min=glm::vec3(0.0,0.0,0.0);
 		max=glm::vec3(0.0,0.0,0.0);
		return;
	}

	int current_mesh=0;
	while(mesh_list[current_mesh]->point_list_size==0)
	{
		current_mesh++;
		if(current_mesh>=mesh_list.size())
  		{
	 		min=glm::vec3(0.0,0.0,0.0);
	 		max=glm::vec3(0.0,0.0,0.0);
			return;
		}
	}

	min.x=mesh_list[0]->point_list[0].x;
	min.y=mesh_list[0]->point_list[0].y;
	min.z=mesh_list[0]->point_list[0].z;

	max.x=mesh_list[0]->point_list[0].x;
	max.y=mesh_list[0]->point_list[0].y;
	max.z=mesh_list[0]->point_list[0].z;

	for(int i=current_mesh;i<mesh_list.size();i++)
	{
		for(int j=1;j<mesh_list[i]->point_list_size;j++)
		{
			if(mesh_list[i]->point_list[j].x<min.x)
				min.x=mesh_list[i]->point_list[j].x;

			if(mesh_list[i]->point_list[j].y<min.y)
				min.y=mesh_list[i]->point_list[j].y;

			if(mesh_list[i]->point_list[j].z<min.z)
				min.z=mesh_list[i]->point_list[j].z;

			if(mesh_list[i]->point_list[j].x>max.x)
				max.x=mesh_list[i]->point_list[j].x;

			if(mesh_list[i]->point_list[j].y>max.y)
				max.y=mesh_list[i]->point_list[j].y;

			if(mesh_list[i]->point_list[j].z>max.z)
				max.z=mesh_list[i]->point_list[j].z;
		}
	}
}

glm::vec3 Object3d::get_size()
{
	glm::vec3 min, max;

	get_min_and_max(min,max);

	return glm::vec3(max.x-min.x,max.y-min.y,max.z-min.z);
}

glm::vec3 Object3d::get_center()
{
	glm::vec3 min, max, resultat;
	get_min_and_max(min,max);

	resultat.x=min.x+(max.x-min.x)/2.0;
	resultat.y=min.y+(max.y-min.y)/2.0;
	resultat.z=min.z+(max.z-min.z)/2.0;

	return resultat;
}

glm::vec3 Object3d::get_center(glm::vec3& min, glm::vec3& max)
{
	glm::vec3 resultat;
	get_min_and_max(min,max);

	resultat.x=min.x+(max.x-min.x)/2.0;
	resultat.y=min.y+(max.y-min.y)/2.0;
	resultat.z=min.z+(max.z-min.z)/2.0;

	return resultat;
}

void Object3d::resize(float size,int axe_code)
{
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;

	center=get_center(min,max);

	float size_x = max.x-min.x;
	float size_y = max.y-min.y;
	float size_z = max.z-min.z;

	float ratio_x = size/size_x;
	float ratio_y = size/size_y;
	float ratio_z = size/size_z;

	// modification de la taille sur l'axe des X
	if((axe_code | CODE_AXE_X)==axe_code)
	{
			for(int i=0;i<mesh_list.size();i++)
			{
				for(int j=0;j<mesh_list[i]->point_list_size;j++)
				{
					mesh_list[i]->point_list[j].x
						=(mesh_list[i]->point_list[j].x-center.x)*ratio_x
							+center.x;

				}
			}
	}

	// modification de la taille sur l'axe des Y
	if((axe_code | CODE_AXE_Y)==axe_code)
	{
			for(int i=0;i<mesh_list.size();i++)
			{
				for(int j=0;j<mesh_list[i]->point_list_size;j++)
				{
					mesh_list[i]->point_list[j].y
						=(mesh_list[i]->point_list[j].y-center.y)*ratio_y
							+center.y;

				}
			}
	}

	// modification de la taille sur l'axe des Z
	if((axe_code | CODE_AXE_Z)==axe_code)
	{
		for(int i=0;i<mesh_list.size();i++)
			{
				for(int j=0;j<mesh_list[i]->point_list_size;j++)
				{
					mesh_list[i]->point_list[j].z
						=(mesh_list[i]->point_list[j].z-center.z)*ratio_z
							+center.z;
				}
			}
	}
}

void Object3d::center(glm::vec3 position_centre)
{
	glm::vec3 centre=get_center();
	glm::vec3 decalage;
 	decalage.x=position_centre.x-centre.x;
 	decalage.y=position_centre.y-centre.y;
 	decalage.z=position_centre.z-centre.z;

	for(int i=0;i<mesh_list.size();i++)
	{
		for(int j=0;j<mesh_list[i]->point_list_size;j++)
		{
			mesh_list[i]->point_list[j]=mesh_list[i]->point_list[j]+decalage;
		}
	}

}

void Object3d::gl_draw()
{
    for(int i=0;i<mesh_list.size();i++)
    {
        mesh_list[i]->gl_draw();
    }
}

void Object3d::gl_draw_normals()
{
    for(int i=0;i<mesh_list.size();i++)
    {
        mesh_list[i]->gl_draw_normals();
    }
}
*/
////////////////////////////////////////////////////////////////////////////////
// Classe TransfoAnim

TransfoAnim::TransfoAnim(float _valeur,float _date_debut,float _date_fin)
				:valeur(_valeur),date_debut(_date_debut),date_fin(_date_fin)
				{}

bool TransfoAnim::finished(float date)
{
    if(date>=date_fin)
    {
        return true;
    }
    else
    {
        return false;
    }
}

TranslationX_Anim::TranslationX_Anim(float _valeur, float _date_debut,float _date_fin)
							:TransfoAnim(_valeur,_date_debut,_date_fin)
							{}

void TranslationX_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// la translation.
	if(date<date_debut) return;

	// Si l'on a dépassé la date de fin, on effectue la translation complète.
	if(date>date_fin)
	{
		glTranslatef(valeur,0.0,0.0);
		return;
	}

	// Interpolation de la translation
	date=(date-date_debut)/(date_fin-date_debut);
	glTranslatef(valeur*date,0.0,0.0);
	return;
}

TranslationY_Anim::TranslationY_Anim(float _valeur, float _date_debut,float _date_fin)
							:TransfoAnim(_valeur,_date_debut,_date_fin)
							{}
void TranslationY_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// la translation.
	if(date<date_debut) return;

	// Si l'on a dépassé la date de fin, on effectue la translation complète.
	if(date>date_fin)
	{
		glTranslatef(0.0,valeur,0.0);
		return;
	}

	// Interpolation de la translation
	date=(date-date_debut)/(date_fin-date_debut);
	glTranslatef(0.0,valeur*date,0.0);
	return;
}

TranslationZ_Anim::TranslationZ_Anim(float _valeur, float _date_debut,float _date_fin)
							:TransfoAnim(_valeur,_date_debut,_date_fin)
							{}

void TranslationZ_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// la translation.
	if(date<date_debut) return;

	// Si l'on a dépassé la date de fin, on effectue la translation complète.
	if(date>date_fin)
	{
		glTranslatef(0.0,0.0,valeur);
		return;
	}

	// Interpolation de la translation
	date=(date-date_debut)/(date_fin-date_debut);
	glTranslatef(0.0,0.0,valeur*date);
	return;
}

EchelleX_Anim::EchelleX_Anim(float _valeur, float _date_debut,float _date_fin)
							:TransfoAnim(_valeur,_date_debut,_date_fin)
							{}

void EchelleX_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// le changement d'échelle.
	if(date<date_debut) return;

	// Si l'on a dépassé la date de fin, on effectue le changement d'échelle
	// complet.
	if(date>date_fin)
	{
		glScalef(1.0+valeur,1.0,1.0);
		return;
	}

	// Interpolation du changement d'échelle.
	date=(date-date_debut)/(date_fin-date_debut);
	glScalef(1.0+valeur*date,1.0,1.0);
	return;
}


EchelleY_Anim::EchelleY_Anim(float _valeur, float _date_debut,float _date_fin)
							:TransfoAnim(_valeur,_date_debut,_date_fin)
							{}

void EchelleY_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// le changement d'échelle.
	if(date<date_debut) return;

	// Si l'on a dépassé la date de fin, on effectue le changement d'échelle
	// complet.
	if(date>date_fin)
	{
		glScalef(1.0,1.0+valeur,1.0);
		return;
	}

	// Interpolation du changement d'échelle.
	date=(date-date_debut)/(date_fin-date_debut);
	glScalef(1.0,1.0+valeur*date,1.0);
	return;
}


EchelleZ_Anim::EchelleZ_Anim(float _valeur, float _date_debut,float _date_fin)
							:TransfoAnim(_valeur,_date_debut,_date_fin)
							{}

void EchelleZ_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// le changement d'échelle.
	if(date<date_debut) return;

	// Si l'on a dépassé la date de fin, on effectue le changement d'échelle
	// complet.
	if(date>date_fin)
	{
		glScalef(1.0,1.0,1.0+valeur);
		return;
	}

	// Interpolation du changement d'échelle.
	date=(date-date_debut)/(date_fin-date_debut);
	glScalef(1.0,1.0,1.0+valeur*date);
	return;
}


Rotation_Anim::Rotation_Anim(float _x, float _y, float _z
								,float _valeur, float _date_debut,float _date_fin)
							:x(_x),y(_y),z(_z)
							,TransfoAnim(_valeur,_date_debut,_date_fin)
							{}

void Rotation_Anim::appliquer(float date)
{
	// Si la date de début n'est pas encore atteinte, on n'applique pas
	// la rotation.
	if(date<date_debut) date=date_debut;

	// Si l'on a dépassé la date de fin, on effectue la rotation
	// complet.
	if(date>date_fin)
	{
		glRotatef(valeur,x,y,z);
		return;
	}

	// Interpolation de la rotation.
	date=(date-date_debut)/(date_fin-date_debut);
	glRotatef(valeur*date,x,y,z);
	return;
}

////////////////////////////////////////////////////////////////////////////////
// classe Liste_TransfoAnim

void Liste_TransfoAnim::lire_translation(ifstream *f,int rang)
{
	char type_transfo;
	float debut;
	float fin;
	float valeur;

	(*f) >> type_transfo;
	(*f) >> valeur;
	(*f) >> debut;
	(*f) >> fin;

	switch(type_transfo)
	{
		case 'x':
			liste[rang]=new TranslationX_Anim(valeur,debut,fin);
		break;
		case 'y':
			liste[rang]=new TranslationY_Anim(valeur,debut,fin);
		break;
		case 'z':
			liste[rang]=new TranslationZ_Anim(valeur,debut,fin);
		break;
	}
}

void Liste_TransfoAnim::lire_echelle(ifstream *f,int rang)
{
	char type_transfo;
	float debut;
	float fin;
	float valeur;

	(*f) >> type_transfo;
	(*f) >> valeur;
	(*f) >> debut;
	(*f) >> fin;

	switch(type_transfo)
	{
		case 'x':
			liste[rang]=new EchelleX_Anim(valeur,debut,fin);
		break;
		case 'y':
			liste[rang]=new EchelleY_Anim(valeur,debut,fin);
		break;
		case 'z':
			liste[rang]=new EchelleZ_Anim(valeur,debut,fin);
		break;
	}
}

void Liste_TransfoAnim::lire_rotation(ifstream *f,int rang)
{
	float debut;
	float fin;
	float valeur;
	float x,y,z;

	(*f) >> x;
	(*f) >> y;
	(*f) >> z;
	(*f) >> valeur;
	(*f) >> debut;
	(*f) >> fin;

	liste[rang]=new Rotation_Anim(x,y,z,valeur,debut,fin);
}

Liste_TransfoAnim::Liste_TransfoAnim(string fichier
												, ThemeFileFinder* file_finder)
{
	string chemin_complet;
	chemin_complet=file_finder->find(REP_ANIMATIONS,fichier);

	ifstream *f;
	f=new ifstream();
	f->open(chemin_complet.c_str(), ios::binary);
	(*f)>>this->duree;
	(*f)>>this->nb_transfo;
	this->max_transfo = this->nb_transfo;
	liste=new TransfoAnim *[max_transfo];

	for(int i=0;i<nb_transfo;i++)
	{
		char type;
		(*f)>>type;

		switch(type)
		{
			case 'T':
				lire_translation(f,i);
			break;
			case 'R':
				lire_rotation(f,i);
			break;
			case 'E':
				lire_echelle(f,i);
			break;
		}
	}
	f->close();
	delete f;
}

Liste_TransfoAnim::Liste_TransfoAnim(int _max_transfo, int _duree): max_transfo(_max_transfo), duree(_duree), nb_transfo(0)
{
	liste=new TransfoAnim *[max_transfo];
}

bool Liste_TransfoAnim::add_translation(char type_transfo,float debut,float fin,float valeur)
{
	// Si le nombre maximum de transformations est atteint, on n'ajoute pas l'animation
	if(this->nb_transfo >=this->max_transfo)
	{
		return false;
	}

	switch(type_transfo)
	{
		case 'x':
			liste[nb_transfo]=new TranslationX_Anim(valeur,debut,fin);
			++nb_transfo;
			return true;
		break;
		case 'y':
			liste[nb_transfo]=new TranslationY_Anim(valeur,debut,fin);
			++nb_transfo;
			return true;
		break;
		case 'z':
			liste[nb_transfo]=new TranslationZ_Anim(valeur,debut,fin);
			++nb_transfo;
			return true;
		break;
	}

	return false;
}

bool Liste_TransfoAnim::add_echelle(char type_transfo,float debut,float fin,float valeur)
{
	if(this->nb_transfo >=this->max_transfo)
	{
		return false;
	}

	switch(type_transfo)
	{
		case 'x':
			liste[nb_transfo]=new EchelleX_Anim(valeur,debut,fin);
			++nb_transfo;
			return true;
		break;
		case 'y':
			liste[nb_transfo]=new EchelleY_Anim(valeur,debut,fin);
			++nb_transfo;
			return true;
		break;
		case 'z':
			liste[nb_transfo]=new EchelleZ_Anim(valeur,debut,fin);
			++nb_transfo;
			return true;
		break;
	}
	return false;
}

bool Liste_TransfoAnim::add_rotation(float debut, float fin, float valeur, float x, float y,float z)
{
	if(this->nb_transfo >=this->max_transfo)
	{
		return false;
	}

	liste[nb_transfo]=new Rotation_Anim(x,y,z,valeur,debut,fin);
	++nb_transfo;
	return true;
}

Liste_TransfoAnim::~Liste_TransfoAnim()
{
	for(int i=0;i<nb_transfo;i++)
	{
		delete liste[i];
	}
	delete [] liste;
	liste=NULL;
	return;
}

void Liste_TransfoAnim::appliquer(float date)
{
	for(int i=0;i<nb_transfo;i++)
	{
		liste[i]->appliquer(date);
	}
	return;
}

bool Liste_TransfoAnim::finished(float date)
{
    for(int i=0;i<nb_transfo;i++)
	{
		if(liste[i]->finished(date)==false) return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Classe Animation

Animation::Animation(Liste_TransfoAnim * _liste_transfo)
			:liste_transfo(_liste_transfo)
			,marche(false),sens(0),pause_mode(false)
   			,date_pause(0),date_debut(0)
	{}

Animation::~Animation()
{
}

void Animation::start(int date)
{
	marche=true;
	date_debut=date;
	return;
}

void Animation::stop()
{
	marche=false;
	return;
}

void Animation::pause(int date)
{
	pause_mode=true;
	date_pause=date;
	return;
}

void Animation::unpause(int date)
{
	pause_mode=false;
	date_debut+=(date-date_pause);
	return;
}

void Animation::inverse()
{
	sens=(sens+1)%2;
	return;
}

void Animation::appliquer(int date)
{
	float temps_ecoule;
	if((date>=date_debut+liste_transfo->duree) || (!marche))
	{
		if(sens) temps_ecoule=0.0;
		else
		temps_ecoule=1.0;
	}
	else
	{
		if(pause_mode) date=date_pause;
		temps_ecoule=((float)(date-date_debut))/((float)(liste_transfo->duree));

		if(sens) temps_ecoule=1.0-temps_ecoule;
	}

	this->liste_transfo->appliquer(temps_ecoule);

	return;
}

int Animation::get_duree()
{
	return this->liste_transfo->duree;
}

////////////////////////////////////////////////////////////////////////////////
// Classe Liste_Animations

Liste_Animations::Liste_Animations(unsigned int _nb_max_animations)
						:nb_max_animations(_nb_max_animations),nb_animations(0)
						,animation_courante(0)
{
	liste=new Animation*[nb_max_animations];
}

Liste_Animations::~Liste_Animations()
{
	for(int i=0;i<nb_animations;i++)
	{
		delete liste[i];
	}
	delete [] liste;
}

bool Liste_Animations::add(Liste_TransfoAnim * _liste_transfo)
{
	if(nb_animations>=nb_max_animations) return false;
	liste[nb_animations]=new Animation(_liste_transfo);
	nb_animations++;
	return true;
}

bool Liste_Animations::set(unsigned int rang_animation)
{
	if(rang_animation>=nb_max_animations) return false;
	animation_courante=rang_animation;
	return true;
}

void Liste_Animations::start(int date)
{
	liste[animation_courante]->start(date);
}

void Liste_Animations::stop()
{
	liste[animation_courante]->stop();
}

void Liste_Animations::pause(int date)
{
	liste[animation_courante]->pause(date);
}

void Liste_Animations::unpause(int date)
{
	liste[animation_courante]->unpause(date);
}

void Liste_Animations::inverse()
{
	liste[animation_courante]->inverse();
}

void Liste_Animations::appliquer(int date)
{
	liste[animation_courante]->appliquer(date);
}

int Liste_Animations::get_duree()
{
	return 	liste[animation_courante]->get_duree();
}
