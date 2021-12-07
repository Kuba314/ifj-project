require "ifj21"

function foo(a:integer):integer, number, number
	return a
end

function main(a:integer)
	local b:integer
	b=foo(a)
	write(b)
end

main(2)
