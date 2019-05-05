require "l_mymath"

--[[
    require的原理
    require "mod"
    会将mod当中的 全局函数和数据放到_G当中
    因此 如果一个mod是全局的 require之后可以直接访问

    但是，如果返回的是局部表，则不能这么访问
--]]

local function test()
    local a = 3
    local b = 4

    local ans = _M:Add(a, b)
    print(ans)

    ans = _M:Sub(a, b)
    print(ans)

    ans = _M:Mul(a, b)
    print(ans)

    ans = _M:Div(a, b)
    print(ans)

end

-- test()

local function test1()
    -- 正确的做法是：
    -- 1.返回局部包
    -- 2.本地接受为local变量
    local _M = require "l_mymath"
    local a = 3
    local b = 4

    local ans = _M:Add(a, b)
    print(ans)

    ans = _M:Sub(a, b)
    print(ans)

    ans = _M:Mul(a, b)
    print(ans)

    ans = _M:Div(a, b)
    print(ans)

end

test1()

local function test2()
    -- 多次打印 一样的值
    -- require 只加载一次
    print(require "l_mymath")
    print(require "l_mymath")
    print(require "l_mymath")
end

test2()
