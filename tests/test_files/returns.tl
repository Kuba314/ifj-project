require "ifj21"

function ret1(): integer
    return 1
end

function ret2(): integer
    local x: integer = 1
    x = 2
    return 3
end

function ret3(): integer
    if true then
        return 1
    end
end

function ret4(): integer
    if true then
        local x: integer = 1
        x = 2
        return 3
    end
end

function ret5()
    local x: integer = 1
    x = 2
    if x == 3 then
        return
    end
end
