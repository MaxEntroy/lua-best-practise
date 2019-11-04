local function traverse(arr)
    local i = 0
    local sz = #arr
    return function()
        i = i + 1
        if i <= sz then
            return i, arr[i]
        end
    end
end

local function main()
    local tb = {11,12,31,14,15}

    for k,v in traverse(tb) do
        print(k, v)
    end
end

main()
