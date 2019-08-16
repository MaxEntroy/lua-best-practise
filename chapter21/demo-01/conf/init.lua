-- lua code that help others
function Sleep(n)
    os.execute("sleep " .. n)
end

function Clear(modname)
    if package.loaded[modname] then
        package.loaded[modname] = nil
    end
end

-- lua code that is called by c
function SetScriptPath(script_path)
    package.path = string.format("%s;%s?.lua", package.path, script_path)
end

function DoTask(arg)
    local task = require "main"
     local ret = task.main(arg)
    return ret
end
