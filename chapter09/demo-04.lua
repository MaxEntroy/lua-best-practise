local function fib(i)
    if i == 1 then return 0
    elseif i == 2  then return 1
    else return fib(i - 1) + fib(i - 2)
    end
end

local function get_fib()
    local f1 = 0
    local f2 = 1
    return function(i)
        if i == 1 then return 0
        elseif i == 2 then return 1
        else
            f1, f2 = f2, f1 + f2
            return f2
        end
    end
end

local function fib2(n)
    local calculus = get_fib()
    local ret = 0
    for i = 1, n do
        ret = calculus(i)
    end
    return ret
end

local function main()
    local n = 8
    local ret = fib(n)
    print("ret is "..ret)

    ret = fib2(n)
    print("ret is "..ret)
end

main()
