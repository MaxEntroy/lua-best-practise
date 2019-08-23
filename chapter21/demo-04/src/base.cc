#include "include/base.h"

#include <iostream>

void StackDump(lua_State* L) {
    std::cout << "-----StackDump called.-----" << std::endl;
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
    std::cout << "-----StackDump finished.-----" << std::endl;
}
