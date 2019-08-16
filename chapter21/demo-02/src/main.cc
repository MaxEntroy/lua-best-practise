#include <iostream>
#include <map>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <gflags/gflags.h>

#include "include/base.h"
#include "include/lua_lib.h"

DEFINE_string(init_path, "", "lua init script");
DEFINE_string(script_path, "", "the lua script path" );

void Init(lua_State* L) {
    lua_pushcfunction(L, Foo);
    lua_setglobal(L, "CFoo");
}

void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, FLAGS_script_path.c_str());
    lua_pcall(L, 1, 0, 0);

    StackDump(L);
    std::string test_msg = "test_msg";
    lua_pushstring(L, test_msg.c_str());
    int test_val = 255;
    lua_pushinteger(L, test_val);
    StackDump(L);

    std::string req_arg = "hello, world";
    lua_getglobal(L, "DoTask");
    StackDump(L);
    lua_pushstring(L, req_arg.c_str());
    StackDump(L);
    lua_pcall(L, 1, 1, 0);
    StackDump(L);

    std::string ret = lua_tostring(L, -1);
    std::cout << ret << std::endl;
    StackDump(L);

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
