#include <map>
#include <string>
#include <utility>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

struct Student {
    int id;
    std::string name;

    Student() : id(-1) {}
    Student(int idx, const std::string& nam) : id(idx), name(nam) {}
};

static void GetAllStudentInfo(std::map<int, Student>& stu_infos) {
    static const std::pair<int, Student> students[] = {
        std::make_pair(101, Student(101, "kang")),
        std::make_pair(102, Student(102, "ruixin")),
        std::make_pair(103, Student(103, "bao")),
    };

    stu_infos = std::map<int, Student>(students, students + sizeof(students)/sizeof(std::pair<int, Student>));
}

static int StudentGetInfo(lua_State* L) {
    int nargs = lua_gettop(L);
    if(nargs != 1) {
        lua_pushliteral(L, "Wrong numbers of arguments.");
        lua_error(L);
    }

    lua_Integer id = 0;
    if(lua_isinteger(L, 1)) {
        id = lua_tointeger(L, 1);
    }
    else{
        lua_pushliteral(L, "Wrong type of arguments.");
        lua_error(L);
    }

    std::map<int, Student> stu_infos;
    GetAllStudentInfo(stu_infos);

    if(stu_infos.find(id) != stu_infos.end()) {
        lua_createtable(L, 0 ,2);

        lua_pushinteger(L, stu_infos[id].id);
        lua_setfield(L, -2, "id");

        lua_pushstring(L, stu_infos[id].name.c_str());
        lua_setfield(L, -2, "name");
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const luaL_Reg student_lib[] = {
    {"GetInfo", StudentGetInfo},
    {NULL, NULL}
};

extern "C" int luaopen_student(lua_State* L) {
    luaL_newlib(L, student_lib);
    return 1;
}

