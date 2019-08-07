print("hello,lua")

function AddAndMinus(x, y)
    return x + y, x - y
end


cal = {}
function cal:Add(x, y)
    return x + y
end

function cal:Minus(x, y)
    return x - y
end
