#ifndef INCLUDE_LUA_LIB_H_
#define INCLUDE_LUA_LIB_H_

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int Foo(lua_State* L);

int SumAndAver(lua_State* L);

#endif
