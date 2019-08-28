function LuaTraceback(msg)
    print("Lua Error: "..msg)
    print(debug.traceback())
    return msg
end

function SetScriptPath(script_path)
    package.path = string.format("%s;%s?.lua", package.path, script_path)
end

function DoTask(arg)
    local task = require "main"
    local ret = task.main(arg)
    return ret
end
