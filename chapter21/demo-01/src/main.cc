#include <iostream>
#include <map>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <gflags/gflags.h>

DEFINE_string(init_path, "", "lua init script");
DEFINE_string(script_path, "", "the lua script path" );

void Init(lua_State* L) {
    int age = 28;
    lua_pushinteger(L, age);
    lua_setglobal(L, "Age");

    std::string name = "lee";
    lua_pushstring(L, name.c_str());
    lua_setglobal(L, "Name");

    int n = lua_gettop(L);
    std::cout << "stack size: " << n << std::endl;
}
void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, FLAGS_script_path.c_str());
    lua_pcall(L, 1, 0, 0);

    std::string req_arg = "hello, world";
    lua_getglobal(L, "DoTask");
    lua_pushstring(L, req_arg.c_str());
    lua_pcall(L, 1, 1, 0);

    std::string ret = lua_tostring(L, -1);
    std::cout << ret << std::endl;

    lua_settop(L, 0);
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    Init(L);
    Driver(L, FLAGS_init_path);

    lua_close(L);
    return 0;
}
