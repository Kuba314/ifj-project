require "ifj21"

function main()
	local a:integer = 2
	local b:integer = 2
	local c:integer = 2
	while a > 0 do
		while c > 0 do
			write(b)
			c = c - 1
		end
		local b:integer = 1
		write(b)
		a = a - 1
	end
end

main()
