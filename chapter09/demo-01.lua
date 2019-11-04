local function foo(n)
    print("foo called "..n)
end

foo(2018)
foo(2018)
foo(2018)

local function func(n)
    local function foo()
        print("func foo called "..n)
        n = n + 1
    end
    return foo
end

local f1 = func(2018)
f1()
f1()
f1()
