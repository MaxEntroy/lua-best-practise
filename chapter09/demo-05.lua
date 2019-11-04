local function Iter(tb)
    local i = 0
    return function()
        i = i + 1
        return tb[i]
    end
end

local function main()
    local tab = {1, 2, 3, 4, 5}
    local iter = Iter(tab)
    local item = iter()
    
    while item do
        print(item)
        item = iter()
    end
    
end

main()
