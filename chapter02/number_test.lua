local function Add(val)
    val = val + 1
    return val
end

local function test()
    local val = 3
    print(val)
    local new_val = Add(val)
    print(val)
    print(new_val)
end

test()
