
-- 3.3 关系
-- lua当中的基础类型：nil, boolean, number, string; table, function, thread, userdata;
-- 前4种为值类型，后4种为引用类型即后4种类型是对象；在Lua当中，对象是以引用的方式存在的，也称作引用类型；
-- 引用类型在 赋值，参数传递，函数返回时均不会引起拷贝，以引用形式操作。
-- 引用类型，本质比较的是地址，because objects do not actually contain these values, only reference to them.

local tb1 = {a = 1, b = 2}
local tb2 = {a = 1, b = 2}

if tb1 == tb2 then
    print("tb1 == tb2")
else
    print("tb1 ~= tb2") -- 不相等
end

print("<------------------------>")

-- string的比较没毛病，就是按值类型，看值是否相等，只不过有一个内化的过程
-- 内化：内容相同的字符串只保留一份。所以，字符串的比较，转化为对于内存地址的比较，时间复杂度O(1)
-- 比较快，别的没什么可说的。

-- 3.4 逻辑运算符
-- 比较特殊，在c/c++当中，逻辑运算符的结果只有true and false。
-- lua当中，逻辑运算的结果可能是运算数，如下规则：
-- 1. a and b   equals a?b:a
-- 2. a or b    equals a?a:b
-- 3. nor a     equals a?false:true
-- ps: nil和false这两个值参与逻辑运算的时候，被转化为false这点和c/c++不同

local c = nil
local d = 0
local e = 100
print(c and d) -->打印nil
print(c and e) -->打印nil
print(d and e) -->打印100

print(c or d) -->打印0
print(c or e) -->打印100

print(not c) -->打印true
print(not d) -->打印false