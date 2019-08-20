#include "include/base.h"
#include "include/lua_lib.h"

#include <iostream>

int Foo(lua_State* L) {
    std::cout << "hello, world!" << std::endl;
    return 0;
}

int SumAndAver(lua_State* L) {
    int n = lua_gettop(L);
    lua_Number sum = 0.0;
    for(int i = 1; i <= n; ++i) {
        if(lua_isnumber(L, i)) {
            lua_Number x = lua_tonumber(L, i);
            sum += x;
        }
        else {
            lua_pushliteral(L, "Invalidd arguments!");
            lua_error(L);
        }
    }
    lua_Number avg = sum / n;
    lua_pushnumber(L, sum);
    lua_pushnumber(L, avg);
    return 2; }
