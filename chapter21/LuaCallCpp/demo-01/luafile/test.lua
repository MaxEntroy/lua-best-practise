print("------------------------------------")
print("test.lua called.")

--[[
for k,v in pairs(c_table) do
    print(k,v)
end
--]]

--[[
--lua 调用c 天然的支持变参
local sum = c_add(1, 2)
print("sum = "..sum)

sum = c_add1(1,2,3)
print("sum = "..sum)

sum = c_add1(1,2,3,4,5);
print("sum = "..sum)
-- ]]
--
print("------------------------------------")
