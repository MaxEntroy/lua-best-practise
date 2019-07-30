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

void Test1(lua_State*& L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    Test1(L, FLAGS_script_path);

    lua_close(L);
    return 0;
}
