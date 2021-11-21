require "ifj21"

function fizzbuzz(up_to: integer)

    for i = 0, i <= up_to, 1 do
        if i % 3 == 0 then
            write("Fizz")
        elseif i % 5 == 0 then
            write("Buzz")
        elseif i % 3 == 0 and i % 5 == 0 then
            write("FizzBuzz")
        else
            write(i)
end
