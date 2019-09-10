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
#include "include/my_lualib.h"

DEFINE_string(init_path, "", "lua init script");
DEFINE_string(script_path, "", "the lua script path" );


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


// 规范表化
static void HandleLuaInit(lua_State* L, const std::string& init_path) {
    luaL_dofile(L, init_path.c_str());

    luaL_openlibs(L);
    luaopen_mylibs(L);
    //ShowPackageLoaded(L);
}

static int HandleLuascript(lua_State* L, const std::string& script_path) {
    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, script_path.c_str());
    int status = lua_pcall(L, 1, 0, 0);
    if(status != LUA_OK) {
        const std::string err_msg = lua_tostring(L, -1);
        l_message("HandleLuascript", err_msg);
        lua_pop(L, 1);
        return 0;
    }

    lua_pushcfunction(L, msghandler);
    int err_index = lua_gettop(L);

    const std::string arg = "c_to_lua_req";
    lua_getglobal(L, "DoTask");
    lua_pushstring(L, arg.c_str());
    status = lua_pcall(L, 1, 1, err_index);
    if(status != LUA_OK) {
        const std::string err_msg = lua_tostring(L, -1);
        l_message("HandleLuascript", err_msg);
        lua_pop(L, 1);
        return 0;
    }

    const std::string ret = lua_tostring(L, -1);
    std::cout << "c: " << ret << std::endl;
    lua_pop(L, 1);
    return 1;
}

static int pmain(lua_State* L) {
    const std::string init_path = lua_tostring(L, 1);
    const std::string script_path = lua_tostring(L, 2);

    HandleLuaInit(L, init_path);
    int status = HandleLuascript(L, script_path);

    if(status)
        lua_pushboolean(L, 1); // singal no errors
    else
        lua_pushboolean(L, 0);

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
    if(status != LUA_OK) Report(L, argv[0]);
    else {
        result = lua_toboolean(L, -1);
        std::cout << "result: " << result << std::endl;
    }

    lua_close(L);
    return (status == LUA_OK && result)?0:1;
}
