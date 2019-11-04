local fact = function(n)
    if n < 0 then return -1
    elseif n == 0 or n == 1 then return n
    else return n * fact(n - 1)
    end
end

local function main()
    local val = 3
    print(fact(val))
end

main()
