require "ifj21"

function coords_from_index(index: integer, width: integer, height: integer): integer, integer

    if index >= integer * height then
        write("error: Index out of bounds")
        return -1, -1
    end

    return index // width, index % width
end
