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

- CAPI

```lua
-- 这个操作会出栈 
void lua_setglobal (lua_State *L, const char *name);

-- 这个操作会入栈
-- 这个操作的意义在于，table这个数据结构在c里面并没有，所以赋予c可以产生lua table的能力，送入栈。
-- 然后从栈中到lua变量
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

- CAPI
```c
// Pushes a C function onto the stack 。 This function receives a pointer to a C function and
// pushes onto the stack a Lua value of type function that,when called,invokes the corresponding
// C function
void lua_pushcfunction (lua_State *L, lua_CFunction f);

// Any function to be callable by Lua must follow the correct protocol to receive its parameters
// and return its results
typedef int (*lua_CFunction) (lua_State *L);


void lua_register (lua_State *L, const char *name, lua_CFunction f);
// Sets the C function f as the new value of global name。It is defined as a macro。
#define lua_register(L, n, f)
 (lua_pushcfunction(L, f), lua_setglobal(L, n))


 // 表相关API汇总
// Creates a new empty table and pushes it onto the stack. Parameter narr is a hint for how many elements the table will have as a sequence; 
// parameter nrec is a hint for how many other elements the table will have. Lua may use these hints to preallocate memory for the new table. 
// This preallocation is useful for performance when you know in advance how many elements the table will have. Otherwise you can use the 
// function lua_newtable.
// 1.这个操作有入栈行为
// 2.narr和nrec分别制定该table的array和hash部分的预分配
 void lua_createtable (lua_State *L, int narr, int nrec);
#define lua_newtable(L) lua_createtable(L, 0, 0)

// Creates a new table and registers there the functions in list l.
// 1.从luaL_xxx中能看出，不是基础API
void luaL_newlib (lua_State *L, const luaL_Reg l[]);
#define luaL_newlib(L,l)  \
  (luaL_checkversion(L), luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

// Creates a new table with a size optimized to store all entries in the array l (but does not actually store them). 
// It is intended to be used in conjunction with luaL_setfuncs 
// It is implemented as a macro. The array l must be the actual array, not a pointer to it.
// 1.这是个宏实现
// 2.l必须是实际的数组地址，不能是指针
// 3.底层table当做array实现
// 4.一般和luaL_setfuncs一起用
void luaL_newlibtable (lua_State *L, const luaL_Reg l[]);
#define luaL_newlibtable(L,l)	\
  lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

// Registers all functions in the array l (see luaL_Reg) into the table on the top of the stack (below optional upvalues, see next).
// When nup is not zero, all functions are created sharing nup upvalues, which must be previously pushed on the stack on top of the library table. 
// These values are popped from the stack after the registration.
void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);

// 补充一组CAPI
// 用来c->lua 遍历table时使用
/*
@@ The following macros supply trivial compatibility for some
** changes in the API. The macros themselves document how to
** change your code to avoid using them.
*/
#define lua_strlen(L,i)		lua_rawlen(L, (i))

#define lua_objlen(L,i)		lua_rawlen(L, (i))

// Returns the raw "length" of the value at the given index: for strings, this is the string length; 
// for tables, this is the result of the length operator ('#') with no metamethods; 
// for userdata, this is the size of the block of memory allocated for the userdata; for other values, it is 0.
size_t lua_rawlen (lua_State *L, int index);

int lua_next (lua_State *L, int index);
/*
Pops a key from the stack, and pushes a key–value pair from the table at the given index (the "next" pair after the given key). 
If there are no more elements in the table, then lua_next returns 0 (and pushes nothing).

A typical traversal looks like this:

     // table is in the stack at index 't'
     lua_pushnil(L);  first key 
     while (lua_next(L, t) != 0) {
       // uses 'key' (at index -2) and 'value' (at index -1)
       printf("%s - %s\n",
              lua_typename(L, lua_type(L, -2)),
              lua_typename(L, lua_type(L, -1)));
       // removes 'value'; keeps 'key' for next iteration
       lua_pop(L, 1);
     }
While traversing a table, do not call lua_tolstring directly on a key, unless you know that the key is actually a string. Recall that lua_tolstring may change the value at the given index; this confuses the next call to lua_next.

See function next for the caveats of modifying the table during its traversal.
*/
```

参考
[lua 原生api解读](https://abaojin.github.io/2017/02/22/lua-native-api/)

## demo-01
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

## demo-03
说几点感受
- 常规的c函数，没有返回多个值的能力
- private local stack，前面提到的清空栈，是对于反馈结果而言，因为参数会在下面，所以最后清空的是参数
```c
// 打出来的结果
// 1,2,3,是参数
// 6,2是返回值
// 参数和返回值的对应顺序都是先入栈/出栈的参数从左侧向右进行匹配
lua: c_to_lua_req_arg
-----StackDump called.-----
2
6
3
2
1
-----StackDump finished.-----
sum: 6.0, avg: 2.0
c: lua_to_c_response
```

整体来说，demo-03的接口设计并不好。
主要尝试了一下知识点：
- c遍历lua table
    - 如何获得lua table的大小
- c返回多级table
- map用pair数组初始化

接口设计不好的问题，我在demo-04当中优化

参考<br>
[Lua C Api lua_gettable 、lua_settable 、lua_next 使用详解](https://www.cnblogs.com/chuanwei-zhang/p/4077247.html)<br>