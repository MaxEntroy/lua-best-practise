# lua扩展C程序
本小结主要是一下几个demo.
- demo-01: 基本case，完成c/cpp->lua的调用
- demo-02

## c调用lua
#### 编译
基本实现，完成了cpp对lua的调用。
需要注意的点：
- gflags引入,第三方库的使用
    - 需要link lphread
- lua的引入，第三方库的使用
    - lua是一个纯c的库，需要extern "C"
    -  -ldl为了解析未定义的"dlopen"等引用

#### lua
```lua
int luaL_dofile (lua_State *L, const char *filename);
Loads and runs the given file. It is defined as the following macro:

(luaL_loadfile(L, filename) || lua_pcall(L, 0, LUA_MULTRET, 0))
It returns false if there are no errors or true in case of errors.
```
这个接口需要注意，加载并执行。只有lua文件被执行，状态才能被机器感知

#### 参考<br>
[lua链接](https://www.bilibili.com/read/cv343751/)

## c访问lua全局变量/全局表字段
这块需要特别注意的是，api操作对于栈有什么变化。比如lua_is/toxxx这种api，其实对于栈没有任何操作。
####lua
```lua
int lua_getglobal (lua_State *L, const char *name);
Pushes onto the stack the value of the global name. Returns the type of that value.
全局字段入栈

lua_Number lua_tonumberx (lua_State *L, int index, int *isnum);
Converts the Lua value at the given index to the C type lua_Number (see lua_Number). The Lua value must be a number or a string convertible to a number (see §3.4.3); otherwise, lua_tonumberx returns 0.
栈上的lua值->c值，但是没有出栈操作

int lua_gettable (lua_State *L, int index);
Pushes onto the stack the value t[k], where t is the value at the given index and k is the value at the top of the stack.
This function pops the key from the stack, pushing the resulting value in its place. As in Lua, this function may trigger a metamethod for the "index" event (see §2.4).
这个函数一次入栈，一次出栈。搞明白出入栈的到底是什么东西。
```

####注意
- 获取表字段步骤
    - 表入栈
    - key入栈(lua_getfiled这种形式不用)
    - lua_gettable调用
    - 获取反馈结果
    - lua_pop
- lua_getfiled使用场景
    - key是string
    - {1,2,3}->{[1] = 1, [2] = 2, [3] = 3}，纯线性表的形式不行

####参考<br>
[lua与c/c++交互](https://www.cnblogs.com/ourroad/p/3220364.html)

## c访问lua全局函数