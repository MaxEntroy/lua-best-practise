local old_print = print
local old_load  = load
local old_pairs = pairs
local _old_g = _G

old_g_value = 100

old_print(_G)
old_print(_ENV._G)
old_print(_ENV)
old_print(_G.old_g_value) old_print(_ENV._G.old_g_value) old_print(_ENV.old_g_value) 


-- 以上代码结果，说明一下问题： 
-- 1. _G _ENV._G _ENV 这3个变量，指向了同一块地址 
-- 2. 新增全局变量，直接注册到全局环境中。所以，尽量不要使用全局变量，如果要使用。也可以考虑更换_ENV再使用 -- 还有如下问题？ 
-- 1. _G 和 _ENV._G有什么区别 
-- 2. _G 和 _ENV有什么区别 local _new_env = {} _new_env.tprint = print _new_env.tpairs = pairs _ENV = _new_env new_g_value = 200 for k,v in old_pairs(_new_env) do old_print(k,v) end old_print(_G) old_print(_ENV._G) old_print(_ENV) old_print(_old_g) -- 以上代码结果若明如下问题： -- 1. _G 本质访问的是 _ENV._G, 因为_old_g保存了全局表，可以打印出地址。但是_ENV._G打印不出来，证明_G会解析到_ENV._G -- 2. _G 就是全局表，_ENV则是当前程序的执行环境（个人理解），默认初始化为_G，但是可以改变。 -- 3. _ENV的改变不会影响_G,除非_ENV本生指向_G (可以打印出来new_g_value，但是无法打印出来old_g_value) -- 官方文档 -- Each reference to a free name uses the _ENV that is visible at that point in the program -- 参考：https://www.lua.org/manual/5.3/manual.html#pdf-load 2.2 -- 简言之： -- _G:   global enviroment -- _ENV: chunk environment, _ENV初始化默认值_ENV