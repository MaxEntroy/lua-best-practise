local _M = {}

function _M.init()
end

-- test c functions
local function TestCFoo()
    CFoo()
end

local function TestCSumAndAver()
    local sum, avg = CSumAndAver(1,2,3)
    print("sum: "..sum..", avg: "..avg)
end
--------------------
--
function _M.main(arg)
    print("lua: "..arg)

    -- TestCFoo()
    TestCSumAndAver()

    local response = "lua_to_c_response"
    return response
end

return _M
