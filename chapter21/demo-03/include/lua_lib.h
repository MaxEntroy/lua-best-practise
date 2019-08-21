#ifndef INCLUDE_LUA_LIB_H_
#define INCLUDE_LUA_LIB_H_

#include <map>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int Foo(lua_State* L);

int SumAndAver(lua_State* L);

struct Student {
    int stu_id;
    std::string stu_name;

    Student() : stu_id(-1) {}
};
// input: 学生id列表
// output: 学生id-信息 hash table
int GetStudentInfo(lua_State* L);
void MakeStudentInfo(std::map<int, Student>& stu_info_table);

#endif
