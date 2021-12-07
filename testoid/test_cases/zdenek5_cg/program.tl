require "ifj21"

function foo(b:integer) : integer
	return b
end

function main(a:integer)
	local c:integer = foo(a)
	write(c)
end

main(2)
