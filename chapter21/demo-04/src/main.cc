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

    lua_register(L, "CSumAndAver", SumAndAver);

    lua_register(L, "CGetStudentInfo", GetStudentInfo);
}

// TODO:
// 1.lua_pcall 异常信息
void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, FLAGS_script_path.c_str());
    lua_pcall(L, 1, 0, 0);

    std::string test_msg = "test_msg";
    lua_pushstring(L, test_msg.c_str());
    int test_val = 255;
    lua_pushinteger(L, test_val);

    std::string req_arg = "c_to_lua_req_arg";
    lua_getglobal(L, "DoTask");
    lua_pushstring(L, req_arg.c_str());
    lua_pcall(L, 1, 1, 0);

    std::string ret = lua_tostring(L, -1);
    std::cout << "c: " << ret << std::endl;

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
