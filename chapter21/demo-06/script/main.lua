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
        
        --[[
        for k, v in pairs(stu_info) do
            print(k,v)
        end
        ]]
        
        if stu_info then
            for k, v in pairs(stu_info) do
                print(k.."->"..v)
            end
        else
            print("lua: No stu_info.")
        end

    end
end

local function TestCAPI()
    print("lua: TestCAPI called.")
    local left = 3
    local right = 4

    local ret = CAPI.Add(left)
    print("lua: left + right = "..ret)

    ret = CAPI.Minus(left, right)
    print("lua: left - right = "..ret)
end

--------------------
function _M.main(arg)
    print("lua: "..arg)
    
    -- TestCFoo()
    -- TestCSumAndAver()
    -- TestCGetStudentInfo()
    TestCAPI()

    local response = "lua_to_c_response"
    return response
end

return _M
