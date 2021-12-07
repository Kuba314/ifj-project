require "ifj21"

function func2(id : number) : number
    write(id, "\n")

    id = id * id * id
    return id
end

function func1(a : string)
    local float : number
    if float == nil then
        float = #a
        write(float)
        local float : number = func2(float)
        write( " * 3 = ", float, "\n")
    else end

    return
end

func1("0123456789")
