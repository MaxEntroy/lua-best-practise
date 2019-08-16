#include "include/base.h"
#include "include/lua_lib.h"

#include <iostream>

int Foo(lua_State* L) {
    std::cout << "hello, world!" << std::endl;
    std::cout << "***lua is go to call StackDump.***" << std::endl;
    StackDump(L);
    std::cout << "***lua finished StackDump ***" << std::endl;
    return 0;
}
