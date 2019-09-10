#include "include/lua_lib.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "include/base.h"

// --------------------------------------------------------------
// Other lib
// --------------------------------------------------------------
static int OtherFoo(lua_State* L) {
    std::cout << "hello, world!" << std::endl;
    return 0;
}

static int OtherSumAndAver(lua_State* L) {
    int n = lua_gettop(L);
    lua_Number sum = 0.0;
    for(int i = 1; i <= n; ++i) {
        if(lua_isnumber(L, i)) {
            lua_Number x = lua_tonumber(L, i);
            sum += x;
        }
        else {
            lua_pushliteral(L, "Invalid arguments!");
            lua_error(L);
        }
    }
    lua_Number avg = sum / n;
    lua_pushnumber(L, sum);
    lua_pushnumber(L, avg);
    return 2;
}

static const LuaL_Reg otherlib[] = {
    {"Foo", OtherFoo},
    {"SumAndAver", OtherSumAndAver},
    {NULL, NULL}
};

int luaopen_other(lua_State* L) {
    luaL_newlib(L, otherlib);
    return 1;
};


// --------------------------------------------------------------
// Student lib
// --------------------------------------------------------------
static void MakeStudentInfo(std::map<int, Student>& stu_info_table) {
    std::pair<int, Student> stu_pair_list[] = {
        std::make_pair(101, Student(101, "kang")),
        std::make_pair(102, Student(102, "bruce")),
        std::make_pair(103, Student(103, "jerry")),
        std::make_pair(104, Student(104, "terry")),
        std::make_pair(105, Student(105, "jaime")),
    };

    stu_info_table = std::map<int, Student>(stu_pair_list, stu_pair_list + sizeof(stu_pair_list)/sizeof(stu_pair_list[0]));
}

static int StudentGetInfo(lua_State* L) { std::map<int, Student> stu_info_table;
    MakeStudentInfo(stu_info_table);

    int id = lua_tointeger(L, -1);
    if(stu_info_table.find(id) != stu_info_table.end()) {
        const Student& s = stu_info_table[id];
        lua_createtable(L, 0, 2);
        lua_pushinteger(L, s.stu_id);
        lua_setfield(L, -2, "id");
        lua_pushstring(L, (s.stu_name).c_str());
        lua_setfield(L, -2, "name");
    }
    else{
        lua_pushnil(L);
    }
    return 1;
}

static const luaL_Reg studentlib[] = {
    {"GetInfo", StudentGetInfo},
    {NULL, NULL},
};

int luaopen_student(lua_State* L) {
    luaL_newlib(L, studentlib);
    return 1;
}

// --------------------------------------------------------------
// MyMath lib
// --------------------------------------------------------------
static int MyMathAdd(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        lua_pushliteral(L, "Wrong numbers of arguments!");
        lua_error(L);
    }

    lua_Number left = 0.0;
    lua_Number right = 0.0;

    if(lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        left = lua_tonumber(L, 1);
        right = lua_tonumber(L, 2);
    }
    else {
        lua_pushliteral(L, "Invalid arguments!");
        lua_error(L);
    }

    lua_Number res = left + right;
    lua_pushnumber(L, res);

    return 1;
}

static int MyMathMinus(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        lua_pushliteral(L, "Wrong numbers of arguments!");
        lua_error(L);
    }

    lua_Number left = 0.0;
    lua_Number right = 0.0;

    if(lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        left = lua_tonumber(L, 1);
        right = lua_tonumber(L, 2);
    }
    else {
        lua_pushliteral(L, "Invalid arguments!");
        lua_error(L);
    }

    lua_Number res = left - right;
    lua_pushnumber(L, res);

    return 1;
}

static const luaL_Reg mymathlib[] = {
    {"Add", MyMathAdd},
    {"Minus", MyMathMinus},
    {NULL, NULL},
};

int luaopen_mymath(lua_State* L) {
    luaL_newlib(L, mymathlib);
    return 1;
}


