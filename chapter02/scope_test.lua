val = 10
local function test_scope()
    val = 100
    local val = 3
    print("in the function, val = "..val) -- 3
    val = val + 1
end

test_scope()
print(val) -- 100


local new_val = 10
local function test_scope1()
    new_val = 100
    local new_val = 3
    print("in the function, new_val = "..new_val) -- 3
    new_val = new_val + 1
end

test_scope1()
print(new_val) -- 100

print("-------------------")
x = 10
do
    local x = x -- 10
    print(x)
    x = x + 1 -- 11
    do
        local x = x + 1 -- 12
        print(x)
        x = x + 1 -- 13
    end
    print(x) -- 11
end
print(x) -- 10