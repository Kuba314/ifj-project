require "ifj21"

function coords_from_index(index: integer, width: integer, height: integer): integer, integer

    if index >= width * height then
        write("error: Index out of bounds")
        return -1, -1
    end

    return index // width, index % width
end

function main()
    local x: integer
    local y: integer

    x, y = coords_from_index(8, 5, 5)
    write("index 8 to 5x5 grid: ", x, " ", y, "\n")
end

main()
