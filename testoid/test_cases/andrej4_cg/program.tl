require "ifj21"

function main ()
local int1 : integer = 1
local int2 : integer = 1
local flt1 : number = 1.0
local flt2 : number = 1.0
local num : number

num = int1 - flt1
num = flt1 - int1
num = int1 - int2
num = flt1 - flt2

num = int1 + flt1
num = flt1 + int1
num = int1 + int2
num = flt1 + flt2

num = int1 * flt1
num = flt1 * int1
num = int1 * int2
num = flt1 * flt2

num = int1 / flt1
num = flt1 / int1
num = int1 / int2
num = flt1 / flt2


local ll : integer = int1 // int2



end

main()
