#include <iostream>
#include <map>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <gflags/gflags.h>

DEFINE_string(script_path, "", "the lua script path" );

void Test(lua_State* L, const std::string& script) {
    luaL_dofile(L, script.c_str());

    int x = 3;
    int y = 4;
    int nargs = 2;
    int nrets = 2;
    lua_getglobal(L, "AddAndMinus");
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    lua_pcall(L, nargs, nrets, 0);

    for(int i = 0; i < nrets; ++i) {
        int ret = lua_tonumber(L, i + 1); // lua_is/toxxx函数属于查询操作，不严格遵循出入栈的顺序
        std::cout << ret << std::endl;
    }

    lua_settop(L, 0);
}

void Test1(lua_State* L, const std::string& script) {
    luaL_dofile(L, script.c_str());

    int x = 3;
    int y = 4;

    lua_getglobal(L, "cal");

    lua_getfield(L, -1, "Add");
    lua_getglobal(L, "cal"); // 写法非常trick，结合lua脚本来看
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pcall(L, 3, 1, 0);
    int ret = lua_tonumber(L, -1);
    lua_pop(L, 1);
    std::cout << "Add = " << ret << std::endl;

    lua_getfield(L, -1, "Minus");
    lua_getglobal(L, "cal");
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pcall(L, 3, 1, 0);
    ret = lua_tonumber(L, -1);
    lua_pop(L, 1);
    std::cout << "Minus = " << ret << std::endl;

    lua_settop(L, 0);
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    //Test(L, FLAGS_script_path);
    Test1(L, FLAGS_script_path);

    lua_close(L);
    return 0;
}
