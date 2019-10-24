#include <iostream>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// --------------------------------------------------------------
// Other lib
// --------------------------------------------------------------
static int OtherFoo(lua_State* L) {
    std::cout << "hello, world!" << std::endl;
    return 0;
}

static int OtherSumAndAver(lua_State* L) {
    int n = lua_gettop(L);
    lua_Number sum = 0.0;
    for(int i = 1; i <= n; ++i) {
        if(lua_isnumber(L, i)) {
            lua_Number x = lua_tonumber(L, i);
            sum += x;
        }
        else {
            lua_pushliteral(L, "Invalid arguments!");
            lua_error(L);
        }
    }
    lua_Number avg = sum / n;
    lua_pushnumber(L, sum);
    lua_pushnumber(L, avg);
    return 2;
}

static const luaL_Reg otherlib[] = {
    {"Foo", OtherFoo},
    {"SumAndAver", OtherSumAndAver},
    {NULL, NULL}
};

int luaopen_other(lua_State* L) {
    luaL_newlib(L, otherlib);
    return 1;
};

}
