#include <cstdio>
#include <iostream>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static void StackDump(lua_State* L) {
    std::cout << "StackDump called." << std::endl;
    int n = lua_gettop(L);
    for(int i = n; i > 0; --i) {
        int t = lua_type(L, i);
        switch(t) {
            case LUA_TBOOLEAN:
                std::cout << lua_toboolean(L, i) << std::endl;
                break;
            case LUA_TNUMBER:
                std::cout << lua_tonumber(L, i) << std::endl;
                break;
            case LUA_TSTRING:
                std::cout << lua_tostring(L, i) << std::endl;
                break;
            default:
                std::cout << lua_typename(L, t) << std::endl;
                break;
        }
    }
}

static void test_lua_settop() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);

    StackDump(L);

    // try 5, 3, 1, 0
    // try -1, -2, -3, -4
    lua_settop(L, 5);
    StackDump(L);

    lua_close(L);
}

static void test_lua_pushvalue() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);

    StackDump(L);

    // try 3, -1, 0
    lua_pushvalue(L, 3);
    StackDump(L);

    lua_close(L);
}

static void test_lua_replace() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);
    lua_pushinteger(L, 4);
    lua_pushinteger(L, 5);

    StackDump(L);

    lua_replace(L, 3);
    StackDump(L);

    lua_close(L);
}

static void test_lua_rotate() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);
    lua_pushinteger(L, 4);
    lua_pushinteger(L, 5);

    StackDump(L);

    // try 2, -2
    lua_rotate(L, 3, -2);
    StackDump(L);

    lua_close(L);
}

static void test_lua_remove() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);
    lua_pushinteger(L, 4);
    lua_pushinteger(L, 5);

    StackDump(L);

    lua_remove(L, 3);
    StackDump(L);

    lua_close(L);
}

static void test_lua_insert() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    StackDump(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);
    lua_pushinteger(L, 4);
    lua_pushinteger(L, 5);

    StackDump(L);

    lua_insert(L, 3);
    StackDump(L);

    lua_close(L);
}

int main(void) {
    //test_lua_settop();
    //test_lua_pushvalue();
    //test_lua_replace();
    //test_lua_rotate();
    //test_lua_remove();
    test_lua_insert();
    return 0;
}
