require "ifj21"

function f2(x : integer): integer, integer
    return x + 1, x - 1
end

function main()
    local x : integer = 1
    local y : integer
    x = 5
    while x < 10 do
        local z : integer
        local w : integer
        
        y, z, w = x, f2(x^x)
        write(y,z,w,"\n")
        x = x + 1
    end
end

main()
