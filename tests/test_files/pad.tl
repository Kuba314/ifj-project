require "ifj21"

function pad(str: string, n_spaces: integer)

    while n_spaces > 0 do
        write(" ")
        n_spaces = n_spaces - 1
    end

    write(str)
end
