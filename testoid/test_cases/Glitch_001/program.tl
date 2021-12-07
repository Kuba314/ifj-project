require "ifj21"

function f2(x : integer): integer
    return x + 1
end

function f1(): integer
    return 1
end

function main()
    local x : integer = 1
    local y : integer = 2 * x
    local z : integer = y + f1()
    write(x, y, z)
    x = f2(y)
    write(x)
    y = x + 1
    z = f2(y) + f2(z)
    write(y, z, "\n")
end

main()
