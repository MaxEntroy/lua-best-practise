local str1 = "i love \"Tencent\""
print(str1)

local str2 = "i love 'Tencent'"
print(str2)

local str3 = 'i love "Tencent"'
print(str3)

local str4 = "hello, \nworld!"
print(str4)

-- [[]] 字符串的优点：
-- 1.不用""
-- 2.不用转义换行
local str5 = [[hello, \nworld!]] -- 转义会失效
print(str5)

local str6 = [[hello, world!]]
print(str6)

local page = [[
<html>
    <head>
        <title>Tencent</title>
    </head>
    
    <body>
        <p>http://qq.com</p>
    </body>
</html>    
]]
print(page)


-- string字符串特点
-- 1. string是常量，不能通过下标进行修改。如果要操作string,采用string库
-- 2. 内化的过程，比较操作O(1)复杂度

local function ToUpperCase(s)
    local ans = string.upper(s)
    return ans
end

local function test()
    local str = "Tencent"
    print(str)
    local str_upper = ToUpperCase(str)
    print(str_upper)
end

test()


-- 字符串操作还需要注意一个点
-- table访问字符串的时候：
-- 1. .跟无双引号成员字符串key
-- 2. 双引号字符串key,用[]访问
local corp = {
    corp_name = "TT",
    people_num = 1000
}
print(corp.corp_name)
print(corp["corp_name"])

local CORP_NAME = "corp_name"
print(corp.CORP_NAME)   -- illegal, nil
print(corp[CORP_NAME])