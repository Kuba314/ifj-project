require "ifj21"

function foo(a:integer, b:integer):integer, integer, number
	return a, b
end

function main(a:integer)
	local b:integer
	a, b=foo(a, 3)
	write(a, b)
end

main(2)
