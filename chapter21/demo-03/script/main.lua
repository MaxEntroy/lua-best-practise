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

    local response = "lua_to_c_response"
    return response
end

return _M
