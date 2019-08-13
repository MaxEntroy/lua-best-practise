local _M = {}

function _M.Sleep(n)
    os.execute("sleep " .. n)
end

return _M
