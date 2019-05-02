// 注意头文件顺序
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <iostream>
#include <map>
#include <string>

#include "inc/Student.h"


void PrintStackSize(lua_State* L) {
    std::cout << "lua stack size : " << lua_gettop(L) << std::endl;
}

static int LuaAdd(lua_State* L) {
    int left = 0, right = 0;

    left = lua_tointeger(L, 1);
    right = lua_tointeger(L, 2);

    int ret = left + right;

    lua_pushinteger(L, ret);

    return 1;
}

static int LuaMinus(lua_State* L) {
    int left = 0, right = 0;

    left = lua_tointeger(L, 1);
    right = lua_tointeger(L, 2);

    int ret = left - right;

    lua_pushinteger(L, ret);

    return 1;
}

static int LuaMul(lua_State* L) {
    int left = 0, right = 0;

    left = lua_tointeger(L, 1);
    right = lua_tointeger(L, 2);

    int ret = left * right;

    lua_pushinteger(L, ret);

    return 1;
}

static int LuaDiv(lua_State* L) {
    int left = 0, right = 0;

    left = lua_tointeger(L, 1);
    right = lua_tointeger(L, 2);

    int ret = left / right;

    lua_pushinteger(L, ret);

    return 1;
}

void LuaAssessFuncInC() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    lua_pushcfunction(L, LuaAdd);
    lua_setglobal(L, "Add");

    lua_pushcfunction(L, LuaMinus);
    lua_setglobal(L, "Minus");

    lua_pushcfunction(L, LuaMul);
    lua_setglobal(L, "Mul");

    lua_pushcfunction(L, LuaDiv);
    lua_setglobal(L, "Div");

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessFuncInC1() {
    // 变量 函数
    // 任何Lua调用的c对象 都必须在Lua环境中进行注册
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    lua_register(L, "Add", LuaAdd);

    lua_register(L, "Minus", LuaMinus);

    lua_register(L, "Mul", LuaMul);

    lua_register(L, "Div", LuaDiv);

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessFuncInC2() {
    // way1: lua_pushcfunction(); lua_setgloblal();
    // way2: lua_register()
    // way3: 借助luaL_Reg批量注册全局函数
    // 前2种方式，在函数较多时，麻烦
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    const luaL_Reg math_funcs[] = {
        {"Add", LuaAdd},
        {"Minus", LuaMinus},
        {"Mul", LuaMul},
        {"Div", LuaDiv},
        {NULL, NULL}
    };

    const luaL_Reg* pfuncs = math_funcs;
    for(;pfuncs->func;++pfuncs) {
        lua_register(L, pfuncs->name, pfuncs->func);
    }

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessFuncInC31() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    lua_newtable(L);
    lua_pushcfunction(L, LuaAdd);
    lua_setfield(L, -2, "Add");

    lua_pushcfunction(L, LuaMinus);
    lua_setfield(L, -2, "Minus");

    lua_pushcfunction(L, LuaMul);
    lua_setfield(L, -2, "Mul");

    lua_pushcfunction(L, LuaDiv);
    lua_setfield(L, -2, "Div");

    lua_setglobal(L, "CMathApi");

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

void LuaAssessFuncInC3() {
    // 上面的方式，注册全局函数是方便的
    // 但是，如果把函数注册到一个表里面
    // 采用如下的方式，非常方便，否则还有很多lua_setfield()操作
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    const luaL_Reg libs[] = {
        {"Add", LuaAdd},
        {"Minus", LuaMinus},
        {"Mul", LuaMul},
        {"Div", LuaDiv},
        {NULL, NULL}
    };

    luaL_newlib(L, libs);
    lua_setglobal(L, "CApi");

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

static int LuaopenCApi(lua_State* L) {
    const luaL_Reg libs[] = {
        {"Add", LuaAdd},
        {"Minus", LuaMinus},
        {"Mul", LuaMul},
        {"Div", LuaDiv},
        {NULL, NULL}
    };
    luaL_checkversion(L);
    luaL_newlib(L, libs);
    return 1;
}

void LuaAssessFuncInC4() {
    // 特别注意：
    // 这种实现形式跟以往不同
    // 上一版的实现，它是放到全局表里面
    // 但是，当前版本是当做module来处理，所以这块具体的细节，还可以再学习下
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const std::string kScriptPath("./luafile/");
    const std::string kScriptName("test.lua");
    const std::string kFileName = kScriptPath + kScriptName;

    luaL_requiref(L, "CApi", LuaopenCApi, 0);
    lua_pop(L, 1);

    luaL_dofile(L, kFileName.c_str());
    PrintStackSize(L);

    lua_close(L);
}

int main(void) {
    //LuaAssessFuncInC();
    //LuaAssessFuncInC1();
    LuaAssessFuncInC2();
    //LuaAssessFuncInC3();
    return 0;
}
