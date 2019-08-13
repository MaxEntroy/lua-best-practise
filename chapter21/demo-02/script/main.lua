local _M = {}

function _M.init()
end

function _M.main(arg)
    print("----------------------------")
    print("lua: "..arg)
    print("Name: "..Name..", Age: "..Age)
    print("----------------------------")
    return arg
end

return _M
