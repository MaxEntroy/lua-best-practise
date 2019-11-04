local function sum(n)
    local sum = 0
    for i = 1, n do
        sum = sum + i
    end
    print("sum is "..sum)
end


local function Inc()
    local sum = 0
    return function(i)
        sum = sum + i
        return sum
    end
end

local function sum1(n)
    local incer = Inc()
    for i = 1, n do
        local ret = incer(i)
        if i == n then
            print("sum1 is "..ret)
        end
    end
end

local function main()
    local n = 100
    sum(n)

    sum1(n)
end

main()
