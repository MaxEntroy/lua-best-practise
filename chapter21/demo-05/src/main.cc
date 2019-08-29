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
    int top = lua_gettop(L);

    lua_getglobal(L, "LuaTraceback");
    if(lua_type(L, -1) != LUA_TFUNCTION) {
        std::cerr << "LuaTraeback not found." << std::endl;
        lua_settop(L, top);
        return;
    }
    int err_handler_index = lua_gettop(L);

    lua_getglobal(L, "SetScriptPath");
    if(lua_type(L, -1) != LUA_TFUNCTION) {
        std::cerr << "SetScriptPath not found." << std::endl;
        lua_settop(L, top);
        return;
    }
    lua_pushstring(L, FLAGS_script_path.c_str());
    int ret_code = lua_pcall(L, 1, 0, err_handler_index);
    if(ret_code != 0) {
        std::cerr << "lua_pcall error, ret_code:  " << ret_code << std::endl;
        lua_settop(L, top);
        return;
    }

    std::string req_arg = "c_to_lua_req_arg";
    lua_getglobal(L, "DoTask");
    if(lua_type(L, -1) != LUA_TFUNCTION) {
        std::cerr << "DoTask not found." << std::endl;
        lua_settop(L, top);
        return;
    }
    lua_pushstring(L, req_arg.c_str());
    ret_code = lua_pcall(L, 1, 1, err_handler_index);
    if(ret_code != 0) {
        std::cerr << "lua_pcall error, ret_code:  " << ret_code << std::endl;
        lua_settop(L, top);
        return;
    }

    std::string ret = lua_tostring(L, -1);
    std::cout << "c: " << ret << std::endl;
    std::cout << "Driver is done." << std::endl;
    lua_settop(L, top);
}

static void report(lua_State* L, const std::string& pname) {
    const std::string msg = lua_tostring(L, -1);
    l_message(pname, msg);
    lua_pop(L, 1);
}

static int pmain(lua_State* L) {
    int result = 0;
    const std::string init_path = lua_tostring(L, 1);
    const std::string script_path = lua_tostring(L, 2);
    std::cout << init_path << std::endl;
    std::cout << script_path << std::endl;

    lua_pushinteger(L, result);
    return 1;
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    lua_State* L = luaL_newstate();
    if(L == NULL) {
        l_message(argv[0], "cannot create state: not enought memory");
        return 1;
    }

    lua_pushcfunction(L, &pmain);
    lua_pushstring(L, FLAGS_init_path.c_str());
    lua_pushstring(L, FLAGS_script_path.c_str());
    int status = lua_pcall(L, 2, 1, 0);

    int result = 0;
    if(status != LUA_OK) report(L, argv[0]);
    else result = lua_tointeger(L, -1);
    std::cout << "result: " << result << std::endl;

    lua_close(L);
    return (status == LUA_OK && result)?0:1;
}
