require "ifj21"

function swap(a: number, b: number): number, number
   return b, a
end

function main () 
	local a : number = 8
	local b : number = 6
	a, b = swap(a, b)
end

main()
