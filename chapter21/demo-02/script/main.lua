local _M = {}

function _M.init()
end

function _M.main(arg)
    print("lua: "..arg)
    CFoo()
    return arg
end

return _M