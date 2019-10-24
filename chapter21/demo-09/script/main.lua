local function TestOtherLib()
    print("lua: TestOtherLib called.")
    local other = require "other"

    other.Foo()

    local sum, avg = other.SumAndAver(1,2,3)
    print("lua: sum: "..sum..", avg: "..avg)
end

local function TestStudentLib()
    print("lua: TestStudentLib called.")
    local student = require "student"

    local stu_id_list = {101, 102, 103, 107}
    for _, id in ipairs(stu_id_list) do
        print("id:---------"..id)
        local stu_info = student.GetInfo(id)
        
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

local function TestMyMathLib()
    print("lua: TestMyMath called.")
    local mymath = require "mymath"

    local left = 3
    local right = 4

    local ret = mymath.Add(left, right)
    print("lua: left + right = "..ret)

    ret = mymath.Minus(left, right)
    print("lua: left - right = "..ret)
end

--------------------

local _M = {}

function _M.init()
    print("lua: init called.")
end

function _M.main(arg)
    print("lua: "..arg)
    
    -- TestOtherLib()
    TestStudentLib()
    -- TestMyMathLib()

    local response = "lua_to_c_response"
    return response
end

return _M
