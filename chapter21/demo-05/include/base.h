#ifndef INCLUDE_BASE_H_
#define INCLUDE_BASE_H_

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

void StackDump(lua_State* L);

#endif
