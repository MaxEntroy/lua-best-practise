-- lua中一共八种数据类型
-- nil, boolean, number, string, table, function, thread, user, userdata

local function ToUpperCase(s)
    local ans = string.upper(s)
    return ans
end

local function test()
    local str = "Tencent"
    print(str)
    local str_upper = ToUpperCase(str)
    print(str_upper)
end

test()