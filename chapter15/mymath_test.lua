-- 如果mymath返回全局变量 以下操作合法
-- 如果是局部变量，需要赋值给本地变量
-- ps:尽量写成local变量，不污染全局环境

require "mymath"

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