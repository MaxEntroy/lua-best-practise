extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// mymath lib
static int MyMathAdd(lua_State* L) {
    int nargs = lua_gettop(L);
    if(nargs != 2) {
        lua_pushliteral(L, "Wrong number of arguments!");
        lua_error(L);
    }

    lua_Number left = 0.0;
    lua_Number right = 0.0;
    if(lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        left = lua_tonumber(L, 1);
        right = lua_tonumber(L, 2);
    }
    else{
        lua_pushliteral(L, "Wrong arguments.");
        lua_error(L);
    }

    lua_Number ret = left + right;
    lua_pushnumber(L, ret);

    return 1;
}

static int MyMathMinus(lua_State* L) {
    int nargs = lua_gettop(L);
    if(nargs != 2) {
        lua_pushliteral(L, "Wrong number of arguments!");
        lua_error(L);
    }

    lua_Number left = 0.0;
    lua_Number right = 0.0;
    if(lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        left = lua_tonumber(L, 1);
        right = lua_tonumber(L, 2);
    }
    else{
        lua_pushliteral(L, "Wrong arguments.");
        lua_error(L);
    }

    lua_Number ret = left - right;
    lua_pushnumber(L, ret);

    return 1;
}

static const luaL_Reg mymath_lib[] = {
    {"Add", MyMathAdd},
    {"Minus", MyMathMinus},
    {NULL, NULL}
};

int luaopen_mymath(lua_State* L) {
    luaL_newlib(L, mymath_lib);
    return 1;
}

}
