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

// 访问全局变量
void Test1(lua_State* L, const std::string& lua_script ) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "name");
    std::string name = lua_tostring(L, -1);
    std::cout << "name: " << name << std::endl;

    lua_getglobal(L, "age");
    int age = lua_tonumber(L, -1);
    std::cout << "age: " << age << std::endl;
}

// 访问全局表变量
void Test2(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "stu");

    std::string field1 = "name";
    lua_pushstring(L, field1.c_str());
    lua_gettable(L, -2);
    std::string value1 = lua_tostring(L, -1);
    lua_pop(L, 1);

    std::string field2 = "age";
    lua_pushstring(L, field2.c_str());
    lua_gettable(L, -2);
    int value2 = lua_tonumber(L, -1);
    lua_pop(L, 1);

    std::cout << field1 << ":" << value1 << std::endl;
    std::cout << field2 << ":" << value2 << std::endl;
}

// 访问全局表变量
void Test21(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "stu");

    std::string field1 = "name";
    lua_getfield(L, -1, field1.c_str());
    std::string value1 = lua_tostring(L, -1);
    lua_pop(L, 1);

    std::string field2 = "age";
    lua_getfield(L, -1, field2.c_str());
    std::string value2 = lua_tostring(L, -1);
    lua_pop(L, 1);

    std::cout << field1 << ":" << value1 << std::endl;
    std::cout << field2 << ":" << value2 << std::endl;
}

// 访问全局表变量
// 线性表型table 不能用lua_getfield
void Test22(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    double sum = 0.0;
    double aver = 0.0;

    lua_getglobal(L, "len");
    int n = lua_tonumber(L, -1);
    lua_settop(L, 0);

    lua_getglobal(L, "grades");
    for(int i = 0; i < n; ++i) {
        lua_pushinteger(L, i + 1);
        lua_gettable(L, -2);
        double x = lua_tonumber(L, -1);
        lua_pop(L, 1);
        sum += x;
    }
    lua_settop(L, 0); // 这里不设置也可以，但是本次的写法L还要传递给别的函数，会相互影响

    aver = sum / n;
    std::cout << "aver is " << aver << std::endl;
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    Test1(L, FLAGS_script_path);
    Test2(L, FLAGS_script_path);
    Test21(L, FLAGS_script_path);
    Test22(L, FLAGS_script_path);

    lua_close(L);
    return 0;
}
