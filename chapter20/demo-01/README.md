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