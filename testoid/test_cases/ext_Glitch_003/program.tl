require "ifj21"

function f2() : integer, number
    return 1, 2.0
end

function foo() : integer, integer
    return f2()
end

function main()
    local a : integer
    local b : integer
    a, b = foo()
    write(a,b,"\n")
end

main()
