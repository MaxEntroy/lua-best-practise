### 闭包

q:什么是闭包？
>简而言之，就是函数内部定义的函数。严格意义来说，还包括了他所能截获的外部变量。外部变量，即外部函数内的局部变量。

q:什么是upvalue
>狭义来说，外部函数的局部变量就是upvalue.

q:如何理解upvalue?
>功能上类似c语言当中的static变量，用来描述函数的状态。所以闭包是有状态的，状态通过upvalue来体现。upvalue的意义在于状态记录和共享。

q:闭包和upvalue的关系？
- 同时返回的多个闭包，共享一个upvalue，修改其中一个，会影响其它
- 不同次返回的闭包，状态独立

q:什么时候使用闭包?
>显然，demo-03和demo-04并不是好的示范。和常规的写法相比，语义不清晰。这两个例子用闭包和上值来记录状态，这么做不建议

我们看一个迭代器的例子。在cpp中，迭代器作为容器和算法的桥梁，提供了对于容器进行遍历的能力。对于迭代器，需要记录容器的状态(位置信息)，这样才能知道当前位置，和下一个位置。
```cpp
#include <iostream>
#include <vector>

int main(void) {
    const int arr[] = {1,2,3,4,5};
    std::vector<int> v(arr, arr + sizeof(arr)/sizeof(arr[0]));

    typedef std::vector<int>::const_iterator iter;
    iter b = v.begin();
    iter e = v.end();

    while(b != e) {
        std::cout << *b << std::endl;
        ++b;
    }

    return 0;
}

```

对于lua，我们可以采用类似的办法，实现一个迭代器。分析上面的迭代器，两部分信息是必须知道，其一是容器对象，其二是索引。前者可以设置为共享上值，后者可以设置为记录状态上值。
```lua
local function Iter(tb)
    -- tb共享数据
    -- i是状态
    local i = 0
    return function()
        i = i + 1
        return tb[i]
    end
end

local function main()
    local tab = {1, 2, 3, 4, 5}
    local iter = Iter(tab)
    local item = iter()
    
    while item do
        print(item)
        item = iter()
    end
    
end

main()
```

### 递归
q:lua写递归需要注意什么？
>尽量采用grammar sugar的写法，否则需要先定义函数变量

```lua
local fact = function(n)
    if n < 0 then return -1
    elseif n == 0 or n == 1 then return n
    else return n * fact(n - 1)
    end
end

local function main()
    local val = 3
    print(fact(val))
end

main()
--[[
lua: demo-07.lua:4: attempt to call a nil value (global 'fact')
stack traceback:
	demo-07.lua:4: in upvalue 'fact'
	demo-07.lua:10: in local 'main'
	demo-07.lua:13: in main chunk
	[C]: in ?
]]
```