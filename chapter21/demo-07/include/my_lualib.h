#ifndef INCLUDE_MY_LUALIB_H_
#define INCLUDE_MY_LUALIB_H_

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define LUA_OTHER_LIBNAME "Other"
int luaopen_other(lua_State* L);

#define LUA_STUDENT_LIBNAME "Student"
int luaopen_student(lua_State* L);

#define LUA_MYMATH_LIBNAME "MyMath"
int luaopen_mymath(lua_State* L);

void luaopen_mylibs(lua_State* L);

#endif
