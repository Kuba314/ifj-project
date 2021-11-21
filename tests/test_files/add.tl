require "ifj21"

function add(a: integer, b: integer): integer
    return a + b
end

function add_local(a: integer, b: integer): integer
    local c: integer = a + b
    return c
end
