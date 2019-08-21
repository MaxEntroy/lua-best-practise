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

local function TestCGetStudentInfo()
    local stu_id_list = {101, 102, 103}
    local stu_info_table = CGetStudentInfo(stu_id_list)
    
    for id, info in pairs(stu_info_table) do
        print("id: "..id)
        for k,v in pairs() do
            print(k,v)
        end
        print("----------------")
    end

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
