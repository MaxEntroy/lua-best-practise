## c提升lua效率

#### 基础
lua可以调用c函数，并不意味着可以调用所有的c函数。后者需要满足如下条件
- lua调用c函数规则
    - c函数必须遵循某种规则来获取参数和返回结果(具有固定的函数标签)
    - c函数必须注册到lua环境中，即必须以一种恰当的方式为lua提供该c函数地址

- private stack
    - 基础
        - lua调用c函数使用了与c调用lua相同类型的栈
        - c函数从栈获取参数，并将结果压入栈
    - 特性(private local stack)
        - 不是全局栈，每个lua调用的c函数，都有其private local stack
        - lua调用c时，第一个参数总是位于局部栈种索引位置1

>无论何时 Lua 调用 C, 被调用的函数都得到一个新的栈,这个栈独立于 C 函数本身
的栈,也独立于之前的 Lua 栈。它里面包含了 Lua 传递给 C 函数的所有参数,而 C 函
数则把要返回的结果放入这个栈以返回给调用者。
这个栈特别重要的一点是，独立于lua global stack，也独立于c函数本身的栈。我理解，这个栈的用途应该是lua和c的**通信栈**

#### lua访问c入栈值

- API

```lua
-- 这个操作会出栈 
void lua_setglobal (lua_State *L, const char *name);

-- 这个操作会入栈
-- 这个操作的意义在于，table这个数据结构在c里面并没有，所以赋予c可以产生lua table的能力，送入栈。
-- 然后从栈种到lua变量
-- 其余变量，c与lua都有对应。比如number,string
void lua_newtable (lua_State *L);

-- 这2个操作设置栈
-- lua_setfield对于table充当线性表无能为力，因为key不是字符串，而是1,2,3,...n
void lua_settable (lua_State *L, int index);
void lua_setfield (lua_State *L, int index, const char *k);
```

#### lua调用c函数

- private local stack
>In order to communicate properly with Lua,a C function must use the following protocol,
which defines the way parameters and results are passed:

>a C function receives its arguments from Lua in its stack in direct order (the first argument is
pushed first) So , when the function starts , lua_gettop(L) returns the number of arguments received by the function
 The first argument (if any) is at index 1 and its last argument is at index lua_gettop(L)。

>To return values to Lua, a C function just pushes them onto the stack, in direct order (the first result is pushed first),and returns the number of results

>Any other value in the stack below the results will be properly discarded by Lua. Like a
Lua function,a C function called by Lua can also return many results

显然，**Any other value in the stack below the results will be properly discarded by Lua**所提到的stack，是private local stack，不是全局栈

## demo-02
- lua热更新
做了热更新的实验，有如下结论
1.如果lua重启，会重新加载库。此时的库里面，并没有之前代码中```require "xx.lua"```的模块。所以，如果服务重启，package.loaded肯定是会被重新加载的。
但是，这就失去了讨论热更新的意义。因为热更新说的就是服务不重启。也就是lua env并不会被重新加载。此时讨论热更新才有意义。
2.需要重新加载
如果没有```clear(modname)```代码，确实无法热更新，实验证实
```lua
-- main.lua
local _M = {}

function _M.init()
end

function _M.main(arg)
    print("lua: "..arg)
    -- print("Name: "..Name..", Age: "..Age)
    print("----------------------------")
    return arg
end

return _M
```
```lua
-- init.lua
function SetScriptPath(script_path)
    package.path = string.format("%s;%s?.lua", package.path, script_path)
end


function Sleep(n)
    os.execute("sleep " .. n)
end

function Clear(modname)
    if package.loaded[modname] then
        package.loaded[modname] = nil
    end
end

function DoTask(arg)
    while true do
        Clear("main")
        local task = require "main"
        local ret = task.main(arg)
        Sleep(1)
    end
    return ret
end
```

- lua code
```Init(L)```和```luaL_dofile()```没有什么关系。```lua_newstate```创建之后，相应的lua环境已经有了。
```Init(L)```是把一些c的参数推到栈里，放到lua全局环境。```ludL_dofile```执行init.lua，也是把相应的函数加载到lua全局环境。

## demo-02
lua调用c函数private local stack，不是全局栈，做了个实验。
```cpp
void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, FLAGS_script_path.c_str());
    lua_pcall(L, 1, 0, 0);

    StackDump(L);
    std::string test_msg = "test_msg";
    lua_pushstring(L, test_msg.c_str());
    int test_val = 255;
    lua_pushinteger(L, test_val);
    StackDump(L);

    std::string req_arg = "hello, world";
    lua_getglobal(L, "DoTask");
    StackDump(L);
    lua_pushstring(L, req_arg.c_str());
    StackDump(L);
    lua_pcall(L, 1, 1, 0);
    StackDump(L);

    std::string ret = lua_tostring(L, -1);
    std::cout << ret << std::endl;
    StackDump(L);

    lua_settop(L, 0);
}
/*
-----StackDump called.-----
-----StackDump finished.-----
-----StackDump called.-----
255
test_msg
-----StackDump finished.-----
-----StackDump called.-----
function
255
test_msg
-----StackDump finished.-----
-----StackDump called.-----
hello, world
function
255
test_msg
-----StackDump finished.-----
lua: hello, world
hello, world!
-----StackDump called.-----
hello, world
255
test_msg
-----StackDump finished.-----
hello, world
-----StackDump called.-----
hello, world
255
test_msg
-----StackDump finished.-----
*/
```
显然，在lua调用的c函数之前，入栈的参数，并没有被清空。证明，清空的是private local stack，而非全局栈。

又进步做了实验，在Lua调用c的函数当中，进行StackDump
```cpp
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

/*
-----StackDump called.-----
-----StackDump finished.-----
-----StackDump called.-----
255
test_msg
-----StackDump finished.-----
-----StackDump called.-----
function
255
test_msg
-----StackDump finished.-----
-----StackDump called.-----
hello, world
function
255
test_msg
-----StackDump finished.-----
lua: hello, world
hello, world!
***lua is go to call StackDump.***
-----StackDump called.-----
-----StackDump finished.-----
***lua finished StackDump ***
-----StackDump called.-----
hello, world
255
test_msg
-----StackDump finished.-----
hello, world
-----StackDump called.-----
hello, world
255
test_msg
-----StackDump finished.-----
*/
```
从结果中可以看到，看起来参数都是L，但是实际栈里的东西不一样。lua进行打印的时候，由于lua调用c函数没有给c参数。
所以，private local stack当中没有任何参数。实际中没有打出任何东西。
这点还挺神奇的，虽然是同一个参数，但是关联的确实是不同的栈。

