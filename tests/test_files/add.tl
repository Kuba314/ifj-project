require "ifj21"

function add(a: integer, b: integer): integer
    return a + b
end

function add_local(a: integer, b: integer): integer
    local c: integer = a + b
    return c
end

write("1 + 2 = ", add(1, 2), "\n")
write("34 + 35 = ", add(34, 35), "\n")
