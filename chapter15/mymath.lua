local _M = {}

function _M:Add(a, b)
    return a+b
end

function _M:Sub(a, b)
    return a-b
end

function _M:Mul(a, b)
    return a*b
end

function _M:Div(a, b)
    if b == 0 then
        return nil
    else
        return a/b
    end
end

return _M