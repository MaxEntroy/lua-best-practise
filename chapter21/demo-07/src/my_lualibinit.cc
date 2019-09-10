#include "include/my_lualib.h"

static const luaL_Reg mylibs[] = {
    {LUA_OTHER_LIBNAME, luaopen_other},
    {LUA_STUDENT_LIBNAME, luaopen_student},
    {LUA_MYMATH_LIBNAME, luaopen_mymath},
    {NULL, NULL}
};

void luaopen_mylibs(lua_State* L) {
    for(const luaL_Reg* p = mylibs; p->func; ++p) {
        luaL_requiref(L, p->name, p->func, 0);
    }
}
