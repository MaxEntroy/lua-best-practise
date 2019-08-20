local _M = {}

function _M.init()
end

-- test c functions
local function TestCFoo()
    CFoo()
end
--

function _M.main(arg)
    print("lua: "..arg)

    TestCFoo()

    return arg
end

return _M
