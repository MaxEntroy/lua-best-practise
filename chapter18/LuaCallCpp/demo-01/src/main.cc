#include <iostream>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static int foo(lua_State* L) {
    std::cout << "foo called." << std::endl;

    std::string name = lua_tostring(L, -1);
    int age = lua_tointeger(L, -2);

    std::cout << "name: " << name << std::endl;
    std::cout << "age: " << age << std::endl;
    std::cout << "stack size: " << lua_gettop(L) << std::endl;

    lua_pushinteger(L, age);
    lua_pushstring(L, name.c_str());

    std::cout << "stack size: " << lua_gettop(L) << std::endl;

    return 2;
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

int main(void) {
    //LuaAssessVarInC();
    //LuaAssessTableInC();
    LuaAssessTableInC1();
    return 0;
}
