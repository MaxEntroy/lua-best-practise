local function func(n)
    local function foo1()
        print("func foo1 called "..n)
    end

    local function foo2()
        print("func foo2 called "..n)
        n = n + 1
    end

    return foo1, foo2
end

local f1, f2 = func(2018)

f1()
f1()
f1()

f2()
f1()

f2()
f1()

local f3 = func(2018)
f3()
