#include "include/lua_lib.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "include/base.h"

int Foo(lua_State* L) {
    std::cout << "hello, world!" << std::endl;
    return 0;
}

int SumAndAver(lua_State* L) {
    int n = lua_gettop(L);
    lua_Number sum = 0.0;
    for(int i = 1; i <= n; ++i) {
        if(lua_isnumber(L, i)) {
            lua_Number x = lua_tonumber(L, i);
            sum += x;
        }
        else {
            lua_pushliteral(L, "Invalidd arguments!");
            lua_error(L);
        }
    }
    lua_Number avg = sum / n;
    lua_pushnumber(L, sum);
    lua_pushnumber(L, avg);
    return 2;
}

int GetStudentInfo(lua_State* L) {
    std::map<int, Student> stu_info_table;
    MakeStudentInfo(stu_info_table);

    int sz = lua_rawlen(L, -1);
    std::vector<int> stu_id_vec(sz);
    for(int i = 1; i <= sz; ++i) {
        lua_pushinteger(L, i);
        lua_gettable(L, -2);
        int stu_id = lua_tointeger(L, -1);
        stu_id_vec[i - 1] = stu_id;

        lua_pop(L, 1);
    }

    lua_createtable(L, 0, sz);
    for(int i = 0; i < sz; ++i) {
        int stu_id = stu_id_vec[i];
        const Student& s = stu_info_table[stu_id];
        lua_createtable(L, 0, 2);
        lua_pushinteger(L, s.stu_id);
        lua_setfield(L, -2, "id");
        lua_pushstring(L, s.stu_name.c_str());
        lua_setfield(L, -2, "name");

        std::string stu_id_str = std::to_string(stu_id);
        lua_setfield(L, -2, stu_id_str.c_str());
    }

    return 1;
}

void MakeStudentInfo(std::map<int, Student>& stu_info_table) {
    std::pair<int, Student> stu_pair_list[] = {
        std::make_pair(101, Student(101, "kang")),
        std::make_pair(102, Student(102, "bruce")),
        std::make_pair(103, Student(103, "jerry")),
        std::make_pair(104, Student(104, "terry")),
        std::make_pair(105, Student(105, "jaime")),
    };

    stu_info_table = std::map<int, Student>(stu_pair_list, stu_pair_list + sizeof(stu_pair_list)/sizeof(stu_pair_list[0]));
}
