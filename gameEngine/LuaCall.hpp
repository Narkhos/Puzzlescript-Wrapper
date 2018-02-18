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
 
#ifndef _LUACALL_
#define _LUACALL_

#include<iostream>
#include<string>
#include<lua.hpp>
#include<lauxlib.h>
#include<lualib.h>
#include <vector>
using namespace std; 

/***********************************
 * Toute la complexité de la communication bidirectionnelle 
 * appel de fonction Lua depuis C++ et appel de fonctions C++ depuis lua est codé ici.
 *
 ***********************************/
 
char* c_str(string s);
int to_lua(lua_State* l,string argtypes,...);


class LuaCall {


public :
  LuaCall(string luafilename);
  /************************************    
   ** CALLING LUA FUNCTIONS FROM C++ **
   ************************************/

  int int_call(string func);
  int int_call(string func, int arg1);
  int int_call(string func, string arg1);
  int generic_int_call(string func,string argtypes, ...);

  void void_call(string func);
  void generic_void_call(string func,string argtypes, ...);

  string generic_string_call(string func,string argtypes, ...);   
  string string_call(string func);
  string string_call(string func, string arg1, int arg2);

  vector<int>* intvector_call(string func);
  vector<int>* generic_intvector_call(string func,string argtypes, ...);
  //vector<string>* get_stringvector();

  /*
  string string_call(string func, int arg1);
  string string_call(string func, string arg1);
  */


  /************************************    
   ** CALLING C++ FUNCTIONS FROM LUA **
   ************************************/

  void make_callable_from_lua(string, int (*)(lua_State*) );
  // you will need to use "return to_lua" to return values easily
private :
  lua_State* lua_state;
};

#else

#include<string>
#include<lua.hpp>
#include<lauxlib.h>
#include<lualib.h>

using namespace std; 
char* c_str(string s);
int to_lua(lua_State* l,string argtypes,...);
class LuaCall;

#endif
