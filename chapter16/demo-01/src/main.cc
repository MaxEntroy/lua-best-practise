#include <cstdio>
#include <iostream>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static void StackDump(lua_State* L) {
    int n = lua_gettop(L);
    for(int i = 1; i <= n; ++i) {
        int t = lua_type(L, i);
        switch(t) {
            case LUA_TBOOLEAN:
                std::cout << lua_toboolean(L, i);
                break;
            case LUA_TNUMBER:
                std::cout << lua_tonumber(L, i);
                break;
            case LUA_TSTRING:
                std::cout << lua_tostring(L, i);
                break;
            default:
                std::cout << lua_typename(L, t);
                break;
        }
    }
}

int main(void) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_close(L);
    return 0;
}
