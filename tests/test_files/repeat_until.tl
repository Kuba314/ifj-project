require "ifj21"

function find_next_multiple_of(n: integer, start: integer): integer

    -- while start % n ~= 0 do
    --     start = start + 1
    -- end

    repeat
        start = start + 1
    until start % n == 0

    return start
end

function find_next_multiple_of_with_break(n: integer, start: integer): integer

    while true do
        start = start + 1
        if start % n == 0 then
            break
        end
    end

    return start
end

write("next multiple of 7 after 8 is ", find_next_multiple_of(7, 8), "\n")
write("next multiple of 13 after 21 is ", find_next_multiple_of_with_break(13, 21), "\n")
