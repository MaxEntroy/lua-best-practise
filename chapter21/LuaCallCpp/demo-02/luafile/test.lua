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

local function testLuaAssessFuncInC3() 
    print("testLuaAssessFuncInC3 called.")
    local left = 10
    local right = 2

    local ret = CMathApi.Add(left, right)
    print("ret = "..ret)
    ret = CMathApi.Minus(left, right)
    print("ret = "..ret)
    ret = CMathApi.Mul(left, right)
    print("ret = "..ret)
    ret = CMathApi.Div(left, right)
    print("ret = "..ret)
end

-- testLuaAssessFuncInC()
testLuaAssessFuncInC3()

print("------------------------------------")
