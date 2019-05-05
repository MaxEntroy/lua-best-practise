local function load_process()
    print("<--------------loaded-------------->g")
    for k,v in pairs(package.loaded) do
        print(k,v)
    end

    print("<----------------preload------------>")
    for k, v in pairs(package.preload) do
        print (k, v)
    end

    print("<----------------path--------------->")
    print(package.path)

    print("<----------------cpath-------------->")
    print(package.cpath)

end

load_process()
