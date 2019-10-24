function SetScriptPath(script_path)
    package.path = string.format("%s;%s?.lua", package.path, script_path)
end

function SetCLibPath(clib_path)
    package.cpath = string.format("%s;%s?.so", package.cpath, clib_path)
end

function DoTask(arg)
    local task = require "main"
    task.init()
    local ret = task.main(arg)
    return ret
end
