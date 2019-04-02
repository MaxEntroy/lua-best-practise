print(_G) print(_ENV) _G.print("hello, _G") _ENV.print("hello, _ENV")

print("<-----------G--------------->")

for k,v in pairs(_G) do
    print(k,v)
end

print("<-----------ENV--------------->")
for k,v in pairs(_ENV) do
    print(k, v)
end

-- lua当中的全局变量var 在语法上被翻译为_ENV.var
-- 可以看到_G当中 _ENV当中都有新增的a

a = 3

print("<-----------G--------------->")

for k,v in pairs(_G) do
    print(k,v)
end

print("<-----------ENV--------------->")
for k,v in pairs(_ENV) do
    print(k, v)
end
print("---------------------")
print(_G.a)
print(_ENV.a)
