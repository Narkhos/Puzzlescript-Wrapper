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
 
#include "LuaCall.hpp"

/***********************************
 * Implémentation de LuaCall.hh utilsiant Lua 5.2
 * et les fonctions de la librairie auxiliaire luaL_...
 * pas de Lunar ou de Luabind....
 *
 *
 *
 *******/

#include <stdlib.h>
#include <string.h> // pour la fonction strlen
#include <stdarg.h> // pour les fonctions à nb d'arguments variables

#include <vector>
#include <string>
using namespace std;

/* use these so as to avoid to create a const string at each call time.
const char* STR_SI = c_str("si");
const char* STR_VOID = c_str("");
const char* STR_S = c_str("s");
const char* STR_I = c_str("i");
const char* STR_IS = c_str("is");
*/


/********************************************
 *  Accesseurs permettant de manipuler des objets C++ depuis lua
 *
 * Principe : un pointeur c++ vers l'objet vide à remplir est converti en (size_t) et transmis à une fonction lua.
 * 
 * La fonction lua appelle ensuite des accesseurs qui remplissent/mettent à jour l'objet.
 *
 * Interface prévue :
 *  intvector_fill : remplissage de tableaux d'entiers
 *
 * Manipulation du modèle pour l'exploration arborescence (utile?)
 *  - update_model(coup) : met à jour le modèle
 *  - rollback_model() : inverse de update_model.
 *        revient à l'état précédent. Le modèle aura à sa charge de conserver les infos permettant la réversibilité
 *
 *
 ********************************************/

int intvector_fill(lua_State *l)
//(int pointeur_vecteur,int val)
{
  int argc = lua_gettop(l);
  if(argc != 2){ cerr<<"fatal error"<<endl; exit(666); }

  size_t pointeur_vecteur = lua_tonumber(l, 1);
  int val = lua_tonumber(l, 2);
  ((vector<int>*)pointeur_vecteur)->push_back(val);

  return 0; // le nombre de valeurs de retour
}

void LuaCall::make_callable_from_lua(string functionName, int  (*functionPointer)(lua_State*) )
{
  cout << "making function "<< functionName<<" callable from lua."<<endl;
  lua_pushcfunction(this->lua_state, functionPointer);
  lua_setglobal(this->lua_state, functionName.c_str());
}

/*********************************    
the first parameter describes the type of further arguments :
 - s : char* (NB : not string, as strings are not "trivially-copyable")
 - i : int
 - to be continued...

usage : 
  return to_lua("si",mystring,myint);

Some explainations for curious people.

In lua, functions can expect several return values
  e.g. : a, b = min_max(5, 2, 10, 3)
Note that returning values to lua should be done the following way :
  lua_pushnumber(l, 12);  // pushing the first return value [to lua's function call stack]
  lua_pushstring(l, "See you space cowboy"); //pushing the second return value
  return 2;  // number of return values
******************************/
int to_lua(lua_State*ls,string argtypes,...) //!\\ it is not part of the LuaCall class
{
  int nbargs=0;
  va_list varg;

  //push des valeurs de retour

  va_start(varg, argtypes); // initialisation de la liste des arguments

  // apparemment, va_start se base sur le dernier argument fice pour trouver les arguments suivants
  for(int i = 0; i < (int)argtypes.size(); i++)
  {
    nbargs++;
    switch(argtypes[i])
    {
      case 's' :
	{
	  char* arg = va_arg(varg, char*);
	  cout << "get string arg=" << arg <<endl;
	  lua_pushstring(ls,arg); 
	  break;
	}
      case 'i':
	{
	  int arg = va_arg(varg,int);
	  lua_pushnumber(ls, arg);
	  break;
	}
      default:
	{
	  cout << "error in argtype strings : expected types should be either 's' or 'i'" << endl;
	  exit(2);
	}
    }
  }

  va_end(varg);

  return nbargs;  // number of return values
}

char* c_str(string s)
{
  char * res = (char*)(s.c_str());
  return  res;
}


LuaCall::LuaCall(string luafilename)
{
  this->lua_state=luaL_newstate();
  luaL_openlibs(this->lua_state);

  make_callable_from_lua("cpp_add_int", intvector_fill );

  if(luaL_dofile(this->lua_state,luafilename.c_str())==1)
  {
    cout << "erreur à l'ouverture du fichier de script lua : '" <<luafilename<<"'"<<endl;  
    exit(1);
  }
  

}


/****************************************
 ** Functions with integer return type **
 ****************************************/


// fonction sans argument
int LuaCall::int_call(string func)
{
  return generic_int_call(func,c_str(""));
}

// fonction à 1 argument int
int LuaCall::int_call(string func,int a1)
{
  return generic_int_call(func,c_str("i"),a1);
}

// fonction à 1 argument string
int LuaCall::int_call(string func,string a1)
{
  return generic_int_call(func,c_str("s"),a1.c_str());
}

// argtypes string composée de "s" et de "i" par exemple pour string et int. cela donne donc "iisi" pour 4 arguments...
int LuaCall::generic_int_call(string func,string argtypes,...)
{
  int nbargs=0;
  va_list varg;
  cout << "calling "<< func <<"("<<argtypes<<")"<<endl;
  lua_getglobal(this->lua_state, func.c_str());

  //push des arguments

  va_start(varg, argtypes); // initialisation de la liste des arguments
  // apparemment, va_start se base sur le dernier argument fice pour trouver les arguments suviants
  for(int i = 0; i < (int)argtypes.size(); i++)
  {
    nbargs++;
    switch (argtypes[i]) 
      {
      case 's' :
	{
	  char* arg = va_arg(varg, char*);
	  cout << "get string arg=" << arg <<endl;
	  lua_pushstring(this->lua_state,arg); 
	  break;
	}
      case 'i' :
	{      
	  int arg = va_arg(varg,int);
	  lua_pushnumber(this->lua_state, arg);
	  break;
	}
      default :
	{
	  cout << "error in argtype strings : expected types should be either 's' or 'i'" << endl;
	  exit(2);
	}
      }

  }
  va_end(varg);


  if( lua_pcall(this->lua_state, nbargs, 1, 0) != 0)
  {
    cout << "impossible de lancer la fonction \"" << func << "\" : " << lua_tostring(this->lua_state, -1) <<endl;
    exit(1);
  }
  if (!lua_isnumber(this->lua_state, -1))
  {
    cout << "function `"<< func <<"' must return a number" << endl;
    exit(3);
  }
  int retour = lua_tonumber(this->lua_state, -1);
  lua_pop(this->lua_state, 1);  
  return retour;
}

/*************************************
 ** Functions with void return type **
 *************************************/

void LuaCall::void_call(string func)
{
  return generic_void_call(func,c_str(""));
}


// argtypes string composée de "s" et de "i" par exemple pour string et int. cela donne donc "iisi" pour 4 arguments...
void LuaCall::generic_void_call(string func,string argtypes,...)
{
  int nbargs=0;
  va_list varg;

  lua_getglobal(this->lua_state, func.c_str());

  //push des arguments

  va_start(varg, argtypes); // initialisation de la liste des arguments
  // apparemment, va_start se base sur le dernier argument fice pour trouver les arguments suviants
  for(int i = 0; i < (int)argtypes.size(); i++)
  {
    nbargs++;
    switch (argtypes[i])
      {
      case 's' :
	{
	  char* arg = va_arg(varg, char*);
	  lua_pushstring(this->lua_state,arg); 
	  break;
	}
      case 'i' :
	{      
	  int arg = va_arg(varg,int);
	  lua_pushnumber(this->lua_state, arg);
	  break;
	}
      default:
	{
	  cout << "error in argtype strings : expected types should be either 's' or 'i'" << endl;
	  exit(2);
	}
    
      }
  }
  va_end(varg);
  

  if( lua_pcall(this->lua_state, nbargs, 1, 0) != 0)
  {
    cout << "impossible de lancer la fonction \"" << func << "\" : " << lua_tostring(this->lua_state, -1) <<endl;
    exit(1);
  }
  if (!lua_isnumber(this->lua_state, -1))
  {
    cout << "function `"<< func <<"' must return a number" << endl;
    exit(3);
  }
}


/***************************************
 ** Functions with string return type **
 ***************************************/

string LuaCall::string_call(string func, string arg1, int arg2)
{
  return this->generic_string_call(func,c_str("si"),arg1.c_str(),arg2);
}


string LuaCall::generic_string_call(string func,string argtypes, ...)
{
  int nbargs=0;
  va_list varg;

  lua_getglobal(this->lua_state, func.c_str());

  //push des arguments

  va_start(varg, argtypes); // initialisation de la liste des arguments

  for(int i = 0; i < (int)argtypes.size(); i++)
  {
    nbargs++;
    switch (argtypes[i])
    {
      case 's' :
	{
	  char* arg = va_arg(varg, char*);
	  lua_pushstring(this->lua_state,arg); 
	  break;
	}
      case 'i' :
	{      
	  int arg = va_arg(varg,int);
	  lua_pushnumber(this->lua_state, arg);
	  break;
	}
      default:
	{
	  cout << "error in argtype strings : expected types should be either 's' or 'i'" << endl;
	  exit(2);
	}
    }
  }
  va_end(varg);


  if( lua_pcall(this->lua_state, nbargs, 1, 0) != 0)
  {
    cout << "impossible de lancer la fonction \"" << func << "\" : " << lua_tostring(this->lua_state, -1) <<endl;
    exit(1);
  }
  if (!lua_isstring(this->lua_state, -1))
  {
    cout << "function `"<< func <<"' must return a string" << endl;
    exit(3);
  }
  const char* retour = lua_tostring(this->lua_state, -1);
  lua_pop(this->lua_state, 1);  
  string* retour2 = new string(retour);
  return *retour2;
}

string LuaCall::string_call(string func)
{
  return generic_string_call(func,c_str(""));
}


/***************************************
 ** Functions with vector<int> return type **
 ***************************************/
//     for table traversal, try lua_next() from https://www.lua.org/manual/5.2/manual.html#4.1


vector<int>* LuaCall::intvector_call(string func)
{
  return this->generic_intvector_call(func,c_str(""));
}

vector<int>* LuaCall::generic_intvector_call(string func,string argtypes, ...)
{
  vector<int>* res = new vector<int>();
  int nbargs=0;
  va_list varg;

  lua_getglobal(this->lua_state, func.c_str());

  // argument 1 : le pointeur vers le vecotr à remplir 
  nbargs=1;  
  lua_pushnumber(this->lua_state, (size_t)res); // ajout d'un argument

  // argumenst 2 à n : arguments optionels
  va_start(varg, argtypes); // initialisation de la liste des arguments

  for(int i = 0; i < (int)argtypes.size(); i++)
  {
    nbargs++;
    switch (argtypes[i])
    {
      case 's' :
	{
	  char* arg = va_arg(varg, char*);
	  lua_pushstring(this->lua_state,arg); 
	  break;
	}
      case 'i' :
	{      
	  int arg = va_arg(varg,int);
	  lua_pushnumber(this->lua_state, arg);
	  break;
	}
      default:
	{
	  cout << "error in argtype strings : expected types should be either 's' or 'i'" << endl;
	  exit(2);
	}
    }
  }
  va_end(varg);
  
 if( lua_pcall(this->lua_state, nbargs, 1, 0) != 0)
  {
    cout << "impossible de lancer la fonction \"" << func << "\" : " << lua_tostring(this->lua_state, -1) <<endl;
    exit(1);
  }

  return res;
}

// pas testé, à refaire à partir de get_intvector
/*vector<string>* LuaCall::get_stringvector()
{
  vector<string>* result = new vector<string>();
  string func = "SendVector";
  lua_getglobal(this->lua_state, func.c_str());



  // if only one return value, and value is a table
  if(lua_gettop(this->lua_state) == 1 && lua_istable(this->lua_state, 1))
  {
    // for each entry in the table
    int len = lua_rawlen(this->lua_state, 1); // formerly lua_objlen
    for(int i=1;i <= len; i++)
    {
      // get the entry to stack
      lua_pushinteger(this->lua_state, i); // crée un objet bidon juste pour 
      lua_gettable(this->lua_state, 1);

      // get table entry as string
      const char *s = lua_tostring(this->lua_state, -1);
      if(s)
      {
        // push the value to the vector
        (*result).push_back(s);
      }

      // remove entry from stack
      lua_pop(this->lua_state,1);
    }
  }
  return result;
  }*/
