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