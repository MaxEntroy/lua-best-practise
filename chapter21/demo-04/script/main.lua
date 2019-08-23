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
    local stu_id_list = {101, 102, 103, 107}
    for _, id in ipairs(stu_id_list) do
        print("id:---------"..id)
        local stu_info = CGetStudentInfo(id)
        if stu_info then
            for k, v in pairs(stu_info) do
                print(k.."->"..v)
            end
        else
            print("No stu_info.")
        end
    end
end
--------------------
--
function _M.main(arg)
    print("lua: "..arg)

    -- TestCFoo()
    -- TestCSumAndAver()
    TestCGetStudentInfo()

    local response = "lua_to_c_response"
    return response
end

return _M
