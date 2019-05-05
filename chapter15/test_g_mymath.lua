require "g_mymath"

--[[
    require的原理
    require "mod"
    会将mod当中的 全局函数和数据放到_G当中
    因此 如果一个mod是全局的 require之后可以直接访问

    这样容易导致一个问题，如果_G当中本来存在同名的mod，容易导致覆盖的问题。
    即之前的同名mod包被覆盖
--]]

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
