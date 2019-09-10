#ifndef INCLUDE_BASE_H_
#define INCLUDE_BASE_H_

#include <stdio.h>
#include <stdlib.h>

#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

void StackDump(lua_State* L);

void ShowPackageLoaded(lua_State* L);
// print an error message
#if !defined(lua_writestringerror)
#define lua_writestringerror(s, p) \
    (fprintf(stderr, (s), (p)), fflush(stderr))
#endif

void l_message(const std::string& pname, const std::string& msg);

#endif
