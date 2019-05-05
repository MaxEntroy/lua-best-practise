## 对于require的学习，明确以下几点
---

- require的目的是什么，有什么用？
    - 为了方便代码管理，通常会把lua代码分成不同的模块，通过require函数，把他们加载进来。

- require "xxx.lua" 的处理流程是什么?
    - 把xxx当中的全局函数和表放到_G当中。
    - 这也表明，xxx.lua当中的全局函数和表，在client当中可以直接使用
    - xxx.lua当中的非全局函数和表，则只能赋值给本地变量访问

- 正确使用require的方式是什么？
    - 模块定义为local，返回该模块
    - client存储该模块到local变量，进一步使用

- require的加载流程是什么？
    - package.loaded
    - package.preload
    - package.path
    - package.cpath

- require有什么坑？
    - require有缓存机制，脚本重启需要特殊的操作
    - 有啥返回啥，没啥返回true

- require的实现机制
    - 参考[6.3 – Modules](https://www.lua.org/manual/5.3/manual.html#pdf-require)


>Loads the given module. The function starts by looking into the package.loaded table to determine whether modname is already loaded. If it is, then require returns the value stored at package.loaded[modname]. Otherwise, it tries to find a loader for the module.

>To find a loader, require is guided by the package.searchers sequence. By changing this sequence, we can change how require looks for a module. The following explanation is based on the default configuration for package.searchers.

>First require queries package.preload[modname]. If it has a value, this value (which must be a function) is the loader. Otherwise require searches for a Lua loader using the path stored in package.path. If that also fails, it searches for a C loader using the path stored in package.cpath. If that also fails, it tries an all-in-one loader (see package.searchers).

>Once a loader is found, require calls the loader with two arguments: modname and an extra value dependent on how it got the loader. (If the loader came from a file, this extra value is the file name.) If the loader returns any non-nil value, require assigns the returned value to package.loaded[modname]. If the loader does not return a non-nil value and has not assigned any value to package.loaded[modname], then require assigns true to this entry. In any case, require returns the final value of package.loaded[modname].

>If there is any error loading or running the module, or if it cannot find any loader for the module, then require raises an error.

- require伪代码
```
function require(name)
    if not package.loaded[name] then
        local loader = findloader(name)
        if loader == nil then
            error("unable to load module" .. name)
        end
        package.loaded[name] = true
        local res = loader(name)
        if res ~= nil then
            package.loaded[name] = res
        end
    end
    return package.loaded[name]
end
```