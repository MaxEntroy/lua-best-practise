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
参考<br>
[lua 原生api解读](https://abaojin.github.io/2017/02/22/lua-native-api/)<br>

#### lua debug
这一小节,是由demo-04引起的问题.
即如何写成安全的代码,支持异常处理的功能,并且能提供更多的stack traceback信息。

- lua_pcall的思考
    - lua_pcall支持message handler
    - Such information cannot be gathered after the return of lua_pcall, since by then the stack has unwound.

即，lua_pcall单独支持一个异常处理函数(error message handler)，这么设计是因为，lua_pcall本身不收集stack traceback信息，所以，只能打印出一条错误信息。但是，如果更多的上下文信息，则无法支持。
比如，一个函数总是被其他模块调用，大部分时间不出错。但是，有些问题总复现。此时如何能判断出当时的调用信息呢？
所以，需要stack traceback信息。(cloud wu认为总是需要stack traceback不是好代码)

- lua debug
    - lua没有内置调试器
    - 但是lua支持了debug库，包括调试器需要的所有原语函数

- lua_pcall的message_handler实现
    - 借助lua debug
    - 做在lua层的原因是，方便调整，否则在cpp层每次需要重新编译
    - 调用
        - 时机: lua_pcall在发生运行时错误时， 这个函数会被调用
        - 参数: 需要自定义，默认会接受lua_pcall返回的错误消息
        - 返回值: 需要自定义，返回值将被 lua_pcall 作为错误消息返回在堆栈上
        - 写法：参数必须要拿到，因为这个有提示作用。至于返回值，可以直接打印不返回。但，此时在宿主中不要再获取，因为没有返回

参考<br>
[Lua debug](https://www.runoob.com/lua/lua-debug.html)<br>
[利用lua_pcall()的errfunc参数调试LUA程序](https://blog.csdn.net/sunshine7858/article/details/12307783)<br>

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

## demo-04
本例采用了更合理的设计。这个设计本身没什么好说的。
但是，本例其实涉及到拿去正排信息的问题，会牵扯出来两个很重要的问题。
- c->lua, lua如果error，怎么异常处理。对应到，lua当中不判断拿到的正排是否存在
- lua->c， c如果error，怎么异常处理。对应到，c当中检索正排的过程出问题。比如map不判断key是否存在

1.对于lua error的异常处理
```lua
local function TestCGetStudentInfo()
    local stu_id_list = {101, 102, 103, 107}
    for _, id in ipairs(stu_id_list) do
        print("id:---------"..id)
        -- 107 stu_info不存在，目前cpp做了异常逻辑，返回nil
        local stu_info = CGetStudentInfo(id)
        for k, v in pairs(stu_info) do
            print(k,v)
        end
        --[[
        if stu_info then
            for k, v in pairs(stu_info) do
                print(k.."->"..v)
            end
        else
            print("No stu_info.")
        end
        ]]
    end
end
--[[
执行结果：
1.lua error之后，后续的代码，lua以及c都不会继续执行。主要是c的流程会被打断。没有做到兼容。相当于把服务搞挂了
lua: c_to_lua_req_arg
id:---------101
name	kang
id	101
id:---------102
name	bruce
id	102
id:---------103
name	jerry
id	103
id:---------107
c: ...onal/lua-best-practise/chapter21/demo-04/script/main.lua:21: bad argument #1 to 'pairs' (table expected, got nil)    
]]
```
根据lua_pcall文档，进行如下改造
```cpp
void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, FLAGS_script_path.c_str());
    lua_pcall(L, 1, 0, 0);

    std::string test_msg = "test_msg";
    lua_pushstring(L, test_msg.c_str());
    int test_val = 255;
    lua_pushinteger(L, test_val);

    std::string req_arg = "c_to_lua_req_arg";
    lua_getglobal(L, "DoTask");
    lua_pushstring(L, req_arg.c_str());
    int ret_code = lua_pcall(L, 1, 1, 0);
    if(ret_code != 0) {
        std::string err_msg = lua_tostring(L, -1);
        std::cerr << "lua_pcall error, ret_code:  " << ret_code << ", err_msg: " << err_msg << std::endl;
    }
    else{
        std::string ret = lua_tostring(L, -1);
        std::cout << "c: " << ret << std::endl;
    }

    std::cout << "Driver is done." << std::endl;
    lua_settop(L, 0);
}
/*
服务正常完成，打出异常信息
当然，其实不这么写，服务也不会挂掉。因为用的是lua_pcall进行调用。
本身不会挂掉
lua: c_to_lua_req_arg
id:---------101
id	101
name	kang
id:---------102
id	102
name	bruce
id:---------103
id	103
name	jerry
id:---------107
lua_pcall error, ret_code:  2, err_msg: ...onal/lua-best-practise/chapter21/demo-04/script/main.lua:21: bad argument #1 to 'pairs' (table expected, got nil)
Driver is done.
*/
```

我们再来看看lua_call()调用的结果
```cpp
void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());

    lua_getglobal(L, "SetScriptPath");
    lua_pushstring(L, FLAGS_script_path.c_str());
    lua_pcall(L, 1, 0, 0);

    std::string test_msg = "test_msg";
    lua_pushstring(L, test_msg.c_str());
    int test_val = 255;
    lua_pushinteger(L, test_val);

    std::string req_arg = "c_to_lua_req_arg";
    lua_getglobal(L, "DoTask");
    lua_pushstring(L, req_arg.c_str());
    lua_call(L, 1, 1);
    // int ret_code = lua_pcall(L, 1, 1, 0);
    // if(ret_code != 0) {
    //     std::string err_msg = lua_tostring(L, -1);
    //     std::cerr << "lua_pcall error, ret_code:  " << ret_code << ", err_msg: " << err_msg << std::endl;
    // }
    // else{
        std::string ret = lua_tostring(L, -1);
        std::cout << "c: " << ret << std::endl;
    // }

    std::cout << "Driver is done." << std::endl;
    lua_settop(L, 0);
}
/*
这个结果就很清晰了.
如果c->lua,但是lua挂了.如果使用lua_call,cpp也会abort
但是,lua_pcall不会.
上面的做法,只是把异常信息,与正常逻辑区别开来,用更合理的方式体现.但是cpp本身并不会挂掉.
lua: c_to_lua_req_arg
id:---------101
id	101
name	kang
id:---------102
id	102
name	bruce
id:---------103
id	103
name	jerry
id:---------107
PANIC: unprotected error in call to Lua API (...onal/lua-best-practise/chapter21/demo-04/script/main.lua:21: bad argument #1 to 'pairs' (table expected, got nil))
Aborted (core dumped)
*/
```

结论,c->lua
- 必须使用lua_pcall
    - 因为使用lua_call如果异常,会调用panic,以及abort函数
    - 宿主退出会影响到整个服务.没有理由,一个进程/线程异常,把其他正常进程/线程搞挂
- 对于异常逻辑和正常逻辑分开处理
- 应该使用err_func来跟踪更多信息

```cpp
int lua_pcall (lua_State *L, int nargs, int nresults, int msgh);
以保护模式调用一个函数。

nargs 和 nresults 的含义与 lua_call 中的相同。 如果在调用过程中没有发生错误， lua_pcall 的行为和 lua_call 完全一致。 但是，如果有错误发生的话， lua_pcall 会捕获它， 然后把唯一的值（错误消息）压栈，然后返回错误码。 同 lua_call 一样， lua_pcall 总是把函数本身和它的参数从栈上移除。

如果 msgh 是 0 ， 返回在栈顶的错误消息就和原始错误消息完全一致。 否则， msgh 就被当成是 错误处理函数 在栈上的索引位置。 （在当前的实现里，这个索引不能是伪索引。） 在发生运行时错误时， 这个函数会被调用而参数就是错误消息。 错误处理函数的返回值将被 lua_pcall 作为错误消息返回在堆栈上。

典型的用法中，错误处理函数被用来给错误消息加上更多的调试信息， 比如栈跟踪信息。 这些信息在 lua_pcall 返回后， 由于栈已经展开，所以收集不到了。

lua_pcall 函数会返回下列常数 （定义在 lua.h 内）中的一个：

LUA_OK (0): 成功。
LUA_ERRRUN: 运行时错误。
LUA_ERRMEM: 内存分配错误。对于这种错，Lua 不会调用错误处理函数。
LUA_ERRERR: 在运行错误处理函数时发生的错误。
LUA_ERRGCMM: 在运行 __gc 元方法时发生的错误。 （这个错误和被调用的函数无关。）

/* thread status */
#define LUA_OK		0
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRGCMM	5
#define LUA_ERRERR	6
```

lua traceback实现
```lua
-- 参数: lua_pcall异常时，返回的错误消息。这个参数默认会传递给err_handler
-- 返回值：错误处理函数的返回值会被lua_pcall作为错误消息返回在stack上
-- ps:返回值，不返回也可以，但是宿主不能获取。为了避免这个问题，我还是返回了
function LuaTraceback(msg)
    print("Lua Error: "..msg)
    print(debug.traceback())
    return msg
end
```

需要在cpp中注册
```cpp
void Driver(lua_State* L, const std::string& lua_script) {
    luaL_dofile(L, lua_script.c_str());
    int top = lua_gettop(L);

    lua_getglobal(L, "LuaTraceback");
    if(lua_type(L, -1) != LUA_TFUNCTION) {
        std::cerr << "LuaTraeback not found." << std::endl;
        lua_settop(L, top);
        return;
    }
    int err_handler_index = lua_gettop(L);

    lua_getglobal(L, "SetScriptPath");
    if(lua_type(L, -1) != LUA_TFUNCTION) {
        std::cerr << "SetScriptPath not found." << std::endl;
        lua_settop(L, top);
        return;
    }
    lua_pushstring(L, FLAGS_script_path.c_str());
    int ret_code = lua_pcall(L, 1, 0, err_handler_index);
    if(ret_code != 0) {
        std::cerr << "lua_pcall error, ret_code:  " << ret_code << std::endl;
        lua_settop(L, top);
        return;
    }

    std::string req_arg = "c_to_lua_req_arg";
    lua_getglobal(L, "DoTask");
    if(lua_type(L, -1) != LUA_TFUNCTION) {
        std::cerr << "DoTask not found." << std::endl;
        lua_settop(L, top);
        return;
    }
    lua_pushstring(L, req_arg.c_str());
    ret_code = lua_pcall(L, 1, 1, err_handler_index);
    if(ret_code != 0) {
        std::cerr << "lua_pcall error, ret_code:  " << ret_code << std::endl;
        lua_settop(L, top);
        return;
    }

    std::string ret = lua_tostring(L, -1);
    std::cout << "c: " << ret << std::endl;
    std::cout << "Driver is done." << std::endl;
    lua_settop(L, top);
}
```