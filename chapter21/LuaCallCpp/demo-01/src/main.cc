#include <iostream>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static int Add(lua_State* L) {
    std::cout << "Add called." << std::endl;
    int left = 0, right = 0;

    left = lua_tointeger(L, 1);
    right = lua_tointeger(L, 2);

    int sum = left + right;

    lua_pushinteger(L, sum);

    return 1;
}

static int Add1(lua_State* L) {
    std::cout << "Add1 called." << std::endl;

    int n = lua_gettop(L);
    int sum = 0;
    for(int i = 1; i <= n; ++i) {
        int tmp = lua_tointeger(L, i);
        sum += tmp;
    }

    lua_pushinteger(L, sum);

    return 1;
}

void PrintStackSize(lua_State* L) {
    std::cout << "lua stack size : " << lua_gettop(L) << std::endl;
}

void LuaAssessVarInC() {

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath = "./luafile/";
    const std::string kScriptName = "test.lua";
    const std::string kFileName = kScriptPath + kScriptName;

    std::string name("kungli");
    lua_pushstring(L, name.c_str());
    PrintStackSize(L);
    lua_setglobal(L, "c_name");
    PrintStackSize(L);

    int age = 18;
    lua_pushinteger(L, age);
    PrintStackSize(L);
    lua_setglobal(L, "c_age");
    PrintStackSize(L);

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessTableInC() {
    // c/c++ 中没有table数据类型
    // c/c++ 使用C API lua_newtable() 产生一个lua table类型
    // c/c++ 使用C APT lua_settable() 将c/c++中变量在table中进行设置

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    lua_newtable(L);
    PrintStackSize(L);

    std::string key("name");
    lua_pushstring(L, key.c_str());
    PrintStackSize(L);

    std::string value("kungli");
    lua_pushstring(L, value.c_str());
    PrintStackSize(L);

    lua_settable(L, -3);
    PrintStackSize(L);

    lua_setglobal(L, "c_table");
    PrintStackSize(L);

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessTableInC1() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    lua_newtable(L);
    PrintStackSize(L);

    std::string value("kungli");
    lua_pushstring(L, value.c_str());
    PrintStackSize(L);

    lua_setfield(L, -2, "name");
    PrintStackSize(L);

    lua_setglobal(L, "c_table");
    PrintStackSize(L);

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessFuncInC() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    lua_pushcfunction(L, Add);
    lua_setglobal(L, "c_add");

    lua_pushcfunction(L, Add1);
    lua_setglobal(L, "c_add1");

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

int main(void) {
    //LuaAssessVarInC();
    //LuaAssessTableInC();
    //LuaAssessTableInC1();
    LuaAssessFuncInC();
    return 0;
}
