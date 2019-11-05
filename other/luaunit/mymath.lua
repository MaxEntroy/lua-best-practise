local _M = {}
_M.VERSION = 1.3

function _M.add(a, b)
    return a + b
end

function _M.minus(a, b)
    return a - b
end

function _M:print()
    print(self.VERSION)
end

return _M

--[[
1.看luaunit单测的写法，需要指定预期值，这样才能通过assertion判断预期值和实际值是否一样
2.想想我的需求：
2.1.exp这种，我是打印出来看结果，起码这样很方便。要么就是挨个conf看，判断字符串是否匹配，这个工作量确实大啊。

--]]
