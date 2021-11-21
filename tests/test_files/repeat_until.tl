require "ifj21"

function find_next_multiple_of(n: integer, start: integer): integer

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
end
