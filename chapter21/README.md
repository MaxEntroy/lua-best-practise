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

#### Lua CAPI的正确用法
这一小节主要参考了云风的blog，给出一些结论
- Lua异常
    - lua 发生了未捕获的异常时，会调用 panic 函数，然后调用 abort() 退出进程
    - 所有CAPI都标明了是否抛出异常
    - lua的异常应该由lua_pcall或者lua_resume来捕获

对于Lua CAPI的异常捕获，正确的做法是**当你调用Lua CAPI时，应该确保在本次调用的层次之上，处于某次lua_pcall或者lua_resume当中**

即，lua_pcall去调用某个过程，在这个过程中有Lua CAPI的调用。常见的case有以下两种：
- C编写Lua库。这种情况通常不用考虑。因为调用Lua CAPI的层次都在都在lua调用之下，而lua调用，又在lua_pcall调用之下
- Lua CAPI遍布在宿主程序中。因为宿主程序不在lua_pcall调用之下，这种case是容易出问题的

对于第二种case，云风给了建议：
>完善的做法是，你应该把你的业务逻辑写到一个 lua_CFunction 中，然后用 lua_pcall 去调用它。而这个代码块的参数则应该用 void * 通过 lua_pushlightuserdata 来传递

显然，这么做是把宿主写成lua_CFunction之后，push_cfunction进lua栈。此时当做一个lua函数去调用的。以前我想不通这点是因为，我理解只有lua_getglobal()一个全局lua函数来调用。
没想到，lua_pushcfunction把一个c function推入lua 栈里，他也就变成了一个lua function,可以再次被调用。

那么，层次应该就变成这样。主业务代码写入lua_CFunction。但是，上层还是需要宿主来调用。这个宿主的逻辑就会简单可控很多。我们在这个层面保证Lua CAPI的正确使用要容易的多。

我们来看lua解释器的例子
```c
/*
** Main body of stand-alone interpreter (to be called in protected mode).
** Reads the options and handles them all.
*/
static int pmain (lua_State *L) {
  int argc = (int)lua_tointeger(L, 1);
  char **argv = (char **)lua_touserdata(L, 2);
  int script;
  int args = collectargs(argv, &script);
  luaL_checkversion(L);  /* check that interpreter has correct version */
  if (argv[0] && argv[0][0]) progname = argv[0];
  if (args == has_error) {  /* bad arg? */
    print_usage(argv[script]);  /* 'script' has index of bad arg. */
    return 0;
  }
  if (args & has_v)  /* option '-v'? */
    print_version();
  if (args & has_E) {  /* option '-E'? */
    lua_pushboolean(L, 1);  /* signal for libraries to ignore env. vars. */
    lua_setfield(L, LUA_REGISTRYINDEX, "LUA_NOENV");
  }
  luaL_openlibs(L);  /* open standard libraries */
  createargtable(L, argv, argc, script);  /* create table 'arg' */
  if (!(args & has_E)) {  /* no option '-E'? */
    if (handle_luainit(L) != LUA_OK)  /* run LUA_INIT */
      return 0;  /* error running LUA_INIT */
  }
  if (!runargs(L, argv, script))  /* execute arguments -e and -l */
    return 0;  /* something failed */
  if (script < argc &&  /* execute main script (if there is one) */
      handle_script(L, argv + script) != LUA_OK)
    return 0;
  if (args & has_i)  /* -i option? */
    doREPL(L);  /* do read-eval-print loop */
  else if (script == argc && !(args & (has_e | has_v))) {  /* no arguments? */
    if (lua_stdin_is_tty()) {  /* running in interactive mode? */
      print_version();
      doREPL(L);  /* do read-eval-print loop */
    }
    else dofile(L, NULL);  /* executes stdin as a file */
  }
  lua_pushboolean(L, 1);  /* signal no errors */
  return 1;
}
```
很明显，主逻辑被写到lua_CFunction当中，我们再来看真正的主函数。
```c
int main (int argc, char **argv) {
  int status, result;
  lua_State *L = luaL_newstate();  /* create state */
  if (L == NULL) {
    l_message(argv[0], "cannot create state: not enough memory");
    return EXIT_FAILURE;
  }
  lua_pushcfunction(L, &pmain);  /* to call 'pmain' in protected mode */
  lua_pushinteger(L, argc);  /* 1st argument */
  lua_pushlightuserdata(L, argv); /* 2nd argument */
  status = lua_pcall(L, 2, 1, 0);  /* do the call */
  result = lua_toboolean(L, -1);  /* get result */
  report(L, status);
  lua_close(L);
  return (result && status == LUA_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}
static int report (lua_State *L, int status) {
  if (status != LUA_OK) {
    const char *msg = lua_tostring(L, -1);
    l_message(progname, msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}
/* 这里分析下这段代码
整个流程很清晰，说一下错误处理的过程。
为什么需要status和result两个返回信息。
1.status用来标识线程的运行状态，具体返回码可参考lua_pcall返回值
2.result用来表示内部逻辑的状态。
举个例子，我这段代码计算1-100内四则运算，但是你给我一个区间之外的参数。
运行状态上来说，进步进行计算都不会运行错误，但是会有逻辑错误，这个状态需要result标识。

总共会有3种状态：
1.运行正确，逻辑正确
2.运行正确，逻辑错误
3.运行错误，逻辑也不可能正确
前2种状态，status == LUA_OK,此时栈顶是用户推入的逻辑结果。
result == lua_toboolean可以获得。
report内部，不在执行。因为执行会发现，逻辑值无法转成char*
第3种状态，运行错误，lua_pcall会把error object会被推入栈顶。通常是error message
此时，result还是可以获得结果，因为字符串可以转成bool值
不过，report此时可以正常工作。
最后的结果，因为 status == LUA_OK && result，可以保证main函数返回结果的逻辑正确性。
*/
```
主逻辑写入lua_CFunction，但是上层还是需要一个简单的宿主来调用。但是，此时的宿主由于没有遍布Lua CAPI。所以就非常好控制了，不容易出错。

到目前为止，我们总共写过两种相互调用代码
- c->lua
    - lua: 参数和返回值均无特殊写法，正常函数操作即可
    - c(调用方): 宿主调用lua_pcall，提前把参数压栈，可调用lua函数压栈，指定参数和返回值个数，从栈中获取返回值
- lua->c
    - lua(调用方): 参数和返回值无特殊写法，正常函数操作即可
    - c: 从栈中获取参数，把返回值压栈
    - 并且需要进行注册，且满足lua_CFunction

对于lua CAPI的正确写法，我们会碰到这么一种情形：
- c->c
    - c(宿主): 宿主调用lua_pcall，提前把参数压栈，可调用c函数压栈，指定参数和返回值，从栈中获取返回值
    - c(被掉)：此时的c，写法非常trick，是类似与lua->c时，c的方式。即，从栈中获取参数，把返回值压入栈中。并且要满足lua_CFunction
    - c(宿主)承担的是c->lua当中的c，c(被调)承担的是lua->c当中的c。
    - 多说一点，此种方式。lua_pcall当中nresults应当与c(被调用)返回值个数一致
        - c(被掉)：不是lua->c，不是private stack. 栈里参数被丢弃是因为lua_pcall执行完会pop参数和function，和private stack丢弃参数没有直接关系
        - c(被掉)：最特殊的一点在于，不是lua->c，但是行为却和lua->c的行为基本一致，从栈里获取参数，把结果压入栈中。但是本质上，不是private stack根本原因在于，栈里的参数，是lua_pcall之前压入lua stack L的，如果是private stack，又怎么会压入全局lua stack L. 所以，看起来非常trick，一定要注意本质区别。

这里非常容易误解的是，一定要注意区分c函数的返回值是指示返回个数，还是正常逻辑的返回值。其实就是区别c函数是不是被压入栈里调用的一类。

```cpp
static int pmain(lua_State* L) {
    const std::string init_path = lua_tostring(L, 1);
    const std::string script_path = lua_tostring(L, 2);

    std::cout << init_path << std::endl;
    std::cout << script_path << std::endl;

    return 1; // 这个值控制栈中实际元素的个数，栈里目前有2个元素，返回1会丢弃1个元素
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    lua_State* L = luaL_newstate();
    if(L == NULL) {
        l_message(argv[0], "cannot create state: not enought memory");
        return 1;
    }

    lua_pushcfunction(L, &pmain);
    lua_pushstring(L, FLAGS_init_path.c_str());
    lua_pushstring(L, FLAGS_script_path.c_str());
    int status = lua_pcall(L, 2, 2, 0); // nresults控制栈中逻辑元素个数，此时逻辑值有2个，即lua_gettop(L) == 2，但是实际值只有1个，多于一个会填充Nil.
    StackDump(L);

    int result = 1;
    if(status != LUA_OK) report(L, argv[0]);
    else result = lua_tointeger(L, -1);
    std::cout << "result: " << result << std::endl;
    lua_close(L);
    return (status == LUA_OK && result)?0:1;
}
```

- lua_CFunction抛出异常

官方文档里面提到，大部分Lua CAPI都会抛出异常。对于我们自己写的lua_CFunction作为lib提供给lua函数使用，如果想抛出异常，应该怎么做呢？(这里一定要注意区分一点，上文讲到的lua debug是Lua CAPI如何捕获一个异常，本节是Lua CAPI如何抛出一个异常)
采用如下函数：
```cpp
int lua_error (lua_State *L);
Generates a Lua error, using the value at the top of the stack as the error object. This function does a long jump, and therefore never returns (see luaL_error).
// 正确用法是：
// 1.先把错误msg入栈
// 2.调用lua_error抛出异常
```
注意这个函数的使用场景：在c函数内部，通过lua_error来抛出错误。我们看一个例子：
```cpp
int Add(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        lua_pushliteral(L, "Wrong numbers of arguments!");
        lua_error(L); // 不用返回值 猜测如文档所说，做了long jump，所以该函数不返回
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
```

参考<br>
[Lua C API 的正确用法](https://blog.codingnow.com/2015/05/lua_c_api.html)<br>


#### c/cpp全局函数批量注册/表化

本节，我自己认为，有一下几个重点
- 为什么是c/cpp全局函数？
- 循环注册/表化的好处是什么？为什么要这么做
- requiref这个函数是干嘛的，不用行不行

先说一下第一个问题，c也不是全局函数，和是否全局没有关系。我们看到lua源码当中c的写法有用到static，会限制文件级别作用域。
这个问题最早想说的是cpp的问题，如果不是static修饰，编译不过。这里说的是cpp实现lib供lua调用。

本节主要设计c函数作为lib，批量注册/表化的能力。对于一些常用API，梳理下接口逻辑
```c
#define luaL_newlib(L,l)  \
  (luaL_checkversion(L), luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

// c->stack，创建一个lua table
// 指定为hash table
// 大小少一个是因为luaL_Reg的最后一个元素是sentinel
#define luaL_newlibtable(L,l)	\
  lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)
 
// Registers all functions in the array l (see luaL_Reg) into the table on the top of the stack (below optional upvalues, see next).
// 这个函数提供一个批量注册表化的能力
LUALIB_API void (luaL_setfuncs) (lua_State *L, const luaL_Reg *l, int nup);
```

简单来说，从API的设计中我们可以看出，lua是希望使用c函数时，把他们放入一个表中，在逻辑上形成一个lib，来供Lua访问
所以，循环注册的好处是，先把全局函数在一个统一的地方定义好。逻辑上对应一个lib。操作也更加方便。表化就是逻辑上统一，功能类似的全局函数放入同一个lib当中。

下面说下```luaL_requiref```这个函数，在此之前，我们先来回顾lua当中的require的作用
>为了方便代码管理,通常会把 lua 代码分成不同的模块,然后在通过 require 函数把它们加载进来
即，require的作用和c/cpp当中的```#include```作用很像，把需要的包，加载进来，这样client代码就可以使用包中提供的变量和函数。
那么，加载进来，是加载到哪呢？是加载到```package.loaded```，当client代码访问mod.func的时候，会到```package.loaded```当中查找mod

好了，对于lua代码，我们可以用require来完成对于包(module/lib)的加载，那么对于c/cpp写的lib，如果我们需要使用他们，应该怎么加载呢？
此时，就用到了luaL_requiref。所以，这个函数的作用，就是把modname指示的lib(通常是一个table,但是里面的function field由c/cpp写成),加载进package.loaded，供lua访问。简单来说，**c/cpp写的lib通过luaL_requiref加载进package.loaded，来供lua使用**

注意，上面的解释没有问题。真正的问题出在，对于lua当中```require```的理解有误，后者会返回需要的module，只不过如果缓存(package.loaded)里有，就直接从缓存返回。
否则，加载到缓存中，再返回，主要是为了避免重复加载的过程耗时。我在demo-07当中犯的错误主要是对于缓存的理解错误，缓存里有，但是也需要从缓存里再加载并返回。所以，
代码还是需要调用```require```

为什么```luaL_openlibs()```里面的库都可以直接访问，是因为它在调用```luaL_requiref```的时候，最后一个参数为1，最后的结果会放入```_G```当中，所以可以直接访问。
但是，我在写``luaopen_mylibs````的时候，没有加载进入```_G```中，所以不能直接访问。

```c
void luaL_requiref (lua_State *L, const char *modname,
                    lua_CFunction openf, int glb);
If modname is not already present in package.loaded, calls function openf with string modname as an argument and sets the call result in package.loaded[modname], as if that function has been called through require.

If glb is true, also stores the module into global modname.

Leaves a copy of the module on the stack.
```

下面我们来看一下源码中对于standard libraries的说明
>The standard Lua libraries provide useful functions that are implemented directly through the C API.
1.Some of these functions provide essential services to the language
2.others provide access to "outside" services (e.g., I/O)
3.and others could be implemented in Lua itself, but are quite useful or have critical performance requirements that deserve an implementation in C

上面说了标准库使用c去设计的原因，最后一点提到了性能。
标准库主要有以下几个
- basic library (§6.1);
- coroutine library (§6.2);
- package library (§6.3);
- string manipulation (§6.4);
- basic UTF-8 support (§6.5);
- table manipulation (§6.6);
- mathematical functions (§6.7) (sin, log, etc.);
- input and output (§6.8);
- operating system facilities (§6.9);
- debug facilities (§6.10).

下面来看下源码当中的实现，首先是最外层```luaL_openlibs```

```c
// luaL_requiref做的事情，package.loaded[modname] = openf(), 只是类比，实现细节不是这样
static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base}, // 从这可以看出，_G就是一个table
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_UTF8LIBNAME, luaopen_utf8},
  {LUA_DBLIBNAME, luaopen_debug},
#if defined(LUA_COMPAT_BITLIB)
  {LUA_BITLIBNAME, luaopen_bit32},
#endif
  {NULL, NULL}
};

LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1); // package.loaded[lib->name] = (lib->func)()
    lua_pop(L, 1);  /* remove lib */
  }
}

```

我们进一步再来看一个openf函数
```c
static const luaL_Reg mathlib[] = {
  {"abs",   math_abs},
  {"acos",  math_acos},
  {"asin",  math_asin},
  {"atan",  math_atan},
  {"ceil",  math_ceil},
  {"cos",   math_cos},
  {"deg",   math_deg},
  {"exp",   math_exp},
  {"tointeger", math_toint},
  {"floor", math_floor},
  {"fmod",   math_fmod},
  {"ult",   math_ult},
  {"log",   math_log},
  {"max",   math_max},
  {"min",   math_min},
  {"modf",   math_modf},
  {"rad",   math_rad},
  {"random",     math_random},
  {"randomseed", math_randomseed},
  {"sin",   math_sin},
  {"sqrt",  math_sqrt},
  {"tan",   math_tan},
  {"type", math_type},
#if defined(LUA_COMPAT_MATHLIB)
  {"atan2", math_atan},
  {"cosh",   math_cosh},
  {"sinh",   math_sinh},
  {"tanh",   math_tanh},
  {"pow",   math_pow},
  {"frexp", math_frexp},
  {"ldexp", math_ldexp},
  {"log10", math_log10},
#endif
  /* placeholders */
  {"pi", NULL},
  {"huge", NULL},
  {"maxinteger", NULL},
  {"mininteger", NULL},
  {NULL, NULL}
};


/*
** Open math library
*/
LUAMOD_API int luaopen_math (lua_State *L) {
  luaL_newlib(L, mathlib); // 这里在栈上创建一个table, 把luaL_Reg mathlib[]当中的函数都注册进来，其实做了表化的工作
  lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, (lua_Number)HUGE_VAL);
  lua_setfield(L, -2, "huge");
  lua_pushinteger(L, LUA_MAXINTEGER);
  lua_setfield(L, -2, "maxinteger");
  lua_pushinteger(L, LUA_MININTEGER);
  lua_setfield(L, -2, "mininteger");
  return 1;
}
```

总结，从上面的做法，我们可以大致看出，源码是这么做的
1.定义```static const luaL_Reg xxxlib[]```
2.写xxxlib的openf函数，```luaopen_xxx```，这个函数做表化
3.最后,调用luaL_requiref(L,modname, openf, glb)加载到package.loaded

那，有一个疑问，不加载进package.loaded是否可以？显然可以，luaL_newlib，lua_setglobal也是可以的。即，把这个lib放入全局表。而不是单独作为一个package.loaded当中的item.那么，源码的做法为什么还要单独放，不放到统一的_G当中呢？说下我的理解哈，我觉得也行，即使放到_G中，我们也可以用```local mod = mod or local mod = _G.mod```来隔离全局变量，并提升性能。但是，这可能不算好的设计。其实我们可以看到_G对应的是```lua_openbase```，即全局表只放最基础的。不同的mod，对于lib function做了逻辑的划分，使逻辑更加清晰。我想，这是源码这么做的初衷。相当于对mod做了逻辑上的隔离，逻辑上自然也就更清晰。

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

对于c中异常的处理，我们先来看一段代码
```cpp
// 一下cpp代码没有判断map key是否存在
int GetStudentInfo(lua_State* L) {
    std::map<int, Student> stu_info_table;
    MakeStudentInfo(stu_info_table);

    int id = lua_tointeger(L, -1);
    const Student& s = stu_info_table[id];
    lua_createtable(L, 0, 2);
    lua_pushinteger(L, s.stu_id);
    lua_setfield(L, -2, "id");
    lua_pushstring(L, (s.stu_name).c_str());
    lua_setfield(L, -2, "name");

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
```

lua当中请求了key不存在的情形
```lua
local function TestCGetStudentInfo()
    local stu_id_list = {101, 102, 103, 107}
    for _, id in ipairs(stu_id_list) do
        print("id:---------"..id)
        local stu_info = CGetStudentInfo(id)
        --[[
        for k, v in pairs(stu_info) do
            print(k,v)
        end
        ]]
        if stu_info then
            for k, v in pairs(stu_info) do
                print(k.."->"..v)
            end
        else
            print("No stu_info.")
        end
    end
end
--[[
lua: c_to_lua_req_arg
id:---------101
id->101
name->kang
id:---------102
id->102
name->bruce
id:---------103
id->103
name->jerry
id:---------107
id->-1
name->
c: lua_to_c_response
Driver is done.
]]
```
分析上面的结果，为什么107没有报异常？
这其实跟map的机制有关系，如果访问map当中某个不存在的key。map会自动加入这一项，value采用该类型默认值。
上面的代码我们可以看到，Student其实采用了默认值。

- demo-06

我们来看下表化的代码
```cpp
// 表化
static void HandleLuainit2(lua_State* L, const std::string& init_path) {
    luaL_dofile(L, init_path.c_str());

    static const luaL_Reg l[] = {
        {"Add", Add},
        {"Minus", Minus},
        {NULL, NULL},
    };

    luaL_newlib(L, l);
    lua_setglobal(L, "CAPI");
}
```

直观来看，确实非常方便。如果c提供大量函数供lua访问.下面的过程显然太麻烦了。

```cpp
lua_createtable()
do 
    lua_pushcfunction()
    lua_setfield()
until all functions is set
lua_setglobal()
```

需要特别注意的一点是，```luaL_newlib()```只是简化了前3步的过程，并且提供了循环注册的能力。但是最后的```lua_setglobal()```的能力并没有提供。

- demo-07

上一小节，实现了基本的表化，以及Lua CAPI 抛出异常的能力。本小节实现一个规范的表化操作。

对比规范的写法，和我的写法
```c
LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1); // 所以基础库都会加载进入_G，因此可以直接访问
    lua_pop(L, 1);  /* remove lib */
  }
}

void luaopen_mylibs(lua_State* L) {
    for(const luaL_Reg* p = mylibs; p->func; ++p) {
        luaL_requiref(L, p->name, p->func, 0); // 我这里没有放入_G当中
    }
}
```

下面我们看到lua的写法，还是需要require的操作才可以
```lua
local function TestMyMathLib()
    print("lua: TestMyMath called.")
    local mymath = require "MyMath"

    local left = 3
    local right = 4

    local ret = mymath.Add(left, right)
    print("lua: left + right = "..ret)

    ret = mymath.Minus(left, right)
    print("lua: left - right = "..ret)
end
```

- demo-08
耗时的一个demo，排查了很多问题，具体可以参考cpp-learning的几个项目。下面主要说下注意点:

1.other.so的编译。lua是c程序，所以other.so必须是c的符号，才可以被识别，否则会有```undefined symbol: luaopen_other```这样的错误，解决办法就是```extern "C"```修饰代码即可。
编译命令,```g++ -fPIC -shared -o other.so other_lib.cc```<br>
2.other.so的路径，需要cpath的支持。<br>
3.使用liblua.so，否则，other.so使用了Lua CAPI，需要这部分objs，如果other.so带着.a编译，会和binary当中的.a冲突<br>