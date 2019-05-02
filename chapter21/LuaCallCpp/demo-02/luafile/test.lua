print("------------------------------------")
print("test.lua called.")



local function testLuaAssessFuncInC()
    print("testLuaAssessFuncInC called.")
    local left  = 10
    local right = 2
    
    local ret = Add(left, right)
    print("ret = "..ret)

    local ret = Minus(left, right)
    print("ret = "..ret)

    local ret = Mul(left, right)
    print("ret = "..ret)
    
    local ret = Div(left, right)
    print("ret = "..ret)
end

local function ttest() 
local left = 10
local right = 2
local ret = 0

local CApi = require "CApi"

ret = CApi.Add(left, right)
print("ret = "..ret)
ret = CApi.Minus(left, right)
print("ret = "..ret)
ret = CApi.Mul(left, right)
print("ret = "..ret)
ret = CApi.Div(left, right)
print("ret = "..ret)
end



testLuaAssessFuncInC()

print("------------------------------------")
