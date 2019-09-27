## package/module

- luaL_openlibs相关

```c
// lualib.h
// 定义了包的名字 以及对应lua_openxx函数
LUAMOD_API int (luaopen_base) (lua_State *L);

#define LUA_COLIBNAME	"coroutine"
LUAMOD_API int (luaopen_coroutine) (lua_State *L);

#define LUA_TABLIBNAME	"table"
LUAMOD_API int (luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME	"io"
LUAMOD_API int (luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME	"os"
LUAMOD_API int (luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUAMOD_API int (luaopen_string) (lua_State *L);

#define LUA_UTF8LIBNAME	"utf8"
LUAMOD_API int (luaopen_utf8) (lua_State *L);

#define LUA_BITLIBNAME	"bit32"
LUAMOD_API int (luaopen_bit32) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUAMOD_API int (luaopen_math) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUAMOD_API int (luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
LUAMOD_API int (luaopen_package) (lua_State *L);


/* open all previous libraries */
LUALIB_API void (luaL_openlibs) (lua_State *L);
```

```c
// linit.c
// luaL_Reg结构存储pname, pfunc
// luaL_requiref函数
static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base},
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
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }
}
```


```c
/*
** Stripped-down 'require': After checking "loaded" table, calls 'openf'
** to open a module, registers the result in 'package.loaded' table and,
** if 'glb' is true, also registers the result in the global table.
** Leaves resulting module on the top.
*/
// 整个逻辑还是比较清晰的
// 我就说下lua_pushvalue的写法 产生一个副本的原因在于，有些操作会把一个值pop stack
// 但是整个值可能后续会用，所以先产生副本，每次操作都是操作副本，这样保证原始值不会pop
// 后续可有继续使用
// 最后没有把module从栈里直接pop掉也合理，因为不知道调用者是否还需要直接操作它，所以先留着，调用者不需要再清除即可
LUALIB_API void luaL_requiref (lua_State *L, const char *modname,
                               lua_CFunction openf, int glb) {
  luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
  lua_getfield(L, -1, modname);  /* _LOADED[modname] */
  if (!lua_toboolean(L, -1)) {  /* package not already loaded? */
    lua_pop(L, 1);  /* remove field */
    lua_pushcfunction(L, openf);
    lua_pushstring(L, modname);  /* argument to open function */
    lua_call(L, 1, 1);  /* call 'openf' to open module */
    lua_pushvalue(L, -1);  /* make copy of module (call result) */
    lua_setfield(L, -3, modname);  /* _LOADED[modname] = module */
  }
  lua_remove(L, -2);  /* remove _LOADED table */
  if (glb) {
    lua_pushvalue(L, -1);  /* copy of module */
    lua_setglobal(L, modname);  /* _G[modname] = module */
  }
}

```

在分析源码的时候，接触到了```LUA_REGISTRYINDEX```这个东西，先简单记录他的作用。
>Lua 提供了一个 注册表， 这是一个预定义出来的表， 可以用来保存任何 C 代码想保存的 Lua 值.

参考<br>
[Lua 与C 交互之LUA_REGISTRYINDEX（3）](https://www.cnblogs.com/zsb517/p/6418929.html)<br>

下面我们来看一个基础的luaopen_xx函数
```c
/*
** Open math library
*/
// 上面 lua_call(L, 1, 1); 其实是一个c->c的过程
// 主调相当于c->lua，提前把函数，参数压入栈
// 被调相当于lua->c，有本质区别，不是private local stack，被调c从栈里拿参数，把结果放入栈中，返回值代表返回结果个数。
LUAMOD_API int luaopen_math (lua_State *L) {
  luaL_newlib(L, mathlib);
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

## light c function
这一切都始于云风的一篇文章，看参考。里面提到了lua_cpcall在lua-5.2之后就废弃了，官方文档建议使用lua_pushcfunction和lua_pcall来进行代替。
原因是lua-5.2支持了light c function. 对于此话，我最初臆想是lua_pushcfunction在以前版本没有支持，而是新版本才支持。后来，追查了两版的api，
发现lua_pushcfunction其实在新老版本都是支持的。

进一步考察源码,
```lua
-- lua-5.1实现
LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
    Closure *cl;
    lua_lock(L);
    luaC_checkGC(L);
    api_checknelems(L, n);
    cl = luaF_newCclosure(L, n, getcurrenv(L));
    cl->c.f = fn;
    L->top -= n;
    while (n--)
      setobj2n(L, &cl->c.upvalue[n], L->top+n);
    setclvalue(L, L->top, cl);
    lua_assert(iswhite(obj2gco(cl)));
    api_incr_top(L);
    lua_unlock(L);
  }
  
-- lua-5.2实现
LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
    lua_lock(L);
    if (n == 0) {
      setfvalue(L->top, fn);
    }
    else {
      Closure *cl;
      api_checknelems(L, n);
      api_check(L, n <= MAXUPVAL, "upvalue index too large");
      luaC_checkGC(L);
      cl = luaF_newCclosure(L, n);
      cl->c.f = fn;
      L->top -= n;
      while (n--)
        setobj2n(L, &cl->c.upvalue[n], L->top + n);
      setclCvalue(L, L->top, cl);
    }
    api_incr_top(L);
    lua_unlock(L);
  }
```
可以很明显发现，当我们调用lua_pushcfunction时，本质上调用的是lua_pushcclosure，传递上值个数为0.但是，在lua-5.1版本当中，对于n==0的情形，并无特殊处理。
我理解开销较大的地方应该是对于GC的check，导致显得不是很light，而后者对于n==0的情形，有了单独的处理。所以，lua-5.2版本的lua_pushcfunction，显得像是 light c function
对于没有上值的需求，我们可以直接使用，lua_pushcfunction and lua_pcall即可。

我们回看lua_cpcall，其实这个函数是为了调用类似pmain这样的c function，他们不需要上值，如果在5.1版本使用lua_pcall，那么势必要进行lua_pushcfunction的操作，而后者并无light c function的优化。这是lua_cpcall存在的原因。注意一点，我们在这里讨论的优化，是针对有无上值时lua_pushcfunction是否优化。和这个c函数是否有参数没有关系，即和lua_pushlightuserdata无关。

参考
[Lua C API 的正确用法](https://blog.codingnow.com/2015/05/lua_c_api.html)<br>