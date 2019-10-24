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
//#include "include/my_lualib.h"

DEFINE_string(init_path, "", "lua init script");
DEFINE_string(script_path, "", "the lua script path" );
DEFINE_string(clib_path, "", "the c lib path");

/*
** Message handler used to run all chunks
*/
static int msghandler (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
      return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
                               luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}

static void Report(lua_State* L, const std::string& pname) {
    const std::string msg = lua_tostring(L, -1);
    l_message(pname, msg);
    lua_pop(L, 1);
}

static void HandleLuaInit(lua_State* L, const std::string& init_path) {
    if(luaL_dofile(L, init_path.c_str())) {
        lua_pushliteral(L, "HandleLuaInit: luaL_dofile raise an error");
        lua_error(L);
    }

    luaL_openlibs(L);
    //luaopen_mylibs(L);
    //ShowPackageLoaded(L);
}

static void HandleLuascript(lua_State* L, const std::string& script_path, const std::string& clib_path) {
    lua_pushcfunction(L, msghandler);
    int err_index = lua_gettop(L);

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, script_path.c_str());
    int status = lua_pcall(L, 1, 0, err_index);
    if(status != LUA_OK) {
        lua_error(L);
        return;
    }

    lua_getglobal(L, "SetCLibPath");
    lua_pushstring(L, clib_path.c_str());
    status = lua_pcall(L, 1, 0, err_index);
    if(status != LUA_OK) {
        lua_error(L);
        return;
    }

    const std::string arg = "c_to_lua_req";
    lua_getglobal(L, "DoTask");
    lua_pushstring(L, arg.c_str());
    status = lua_pcall(L, 1, 1, err_index);
    if(status != LUA_OK) {
        lua_error(L);
        return;
    }

    const std::string ret = lua_tostring(L, -1);
    std::cout << "c: " << ret << std::endl;
    lua_pop(L, 1);
}

static int pmain(lua_State* L) {
    const std::string init_path = lua_tostring(L, 1);
    const std::string script_path = lua_tostring(L, 2);
    const std::string clib_path = lua_tostring(L, 3);

    HandleLuaInit(L, init_path);
    HandleLuascript(L, script_path, clib_path);

    return 0;
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
    lua_pushstring(L, FLAGS_clib_path.c_str());
    int status = lua_pcall(L, 3, 1, 0);

    if(status != LUA_OK) Report(L, argv[0]);

    lua_close(L);
    return (status == LUA_OK)?0:1;
}
