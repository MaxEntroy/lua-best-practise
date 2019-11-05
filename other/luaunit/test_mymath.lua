local math = require "mymath"
local lu = require "luaunit"

TestMyMath = {}

    function TestMyMath.test_add_01()
        lu.assertEquals(math.add(1,2), 3)
        lu.assertEquals(math.add(3,5), 8)
    end

    function TestMyMath.test_print()
        math:print()
    end

os.exit( lu.LuaUnit.run() )
