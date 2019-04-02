-- _G只是一张普通的全局表，和tb2没有本质区别，只是他存储了全局环境的值
for k,v in pairs(_G) do
    print(k,v)
end

print(_G)

local tb1 = {a=1, b=2}
print(tb1)

tb2 = {a=1, b=3}
print(tb2)

-- _G类似于c++当中的全局匿名空间，在lua中，相当于全局有名空间
-- _G作为全局表，类比c++当中的全局命名空间，功能上类似于一个基础库
_G.print("hello,lua")
print("hello, this is lua.")