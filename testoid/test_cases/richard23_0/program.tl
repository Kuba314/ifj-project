require "ifj21"

function najvacsi_numb(a: number, b: number, c: number): number
   if a >= b then
	if a >= c then
		return a
	else
		return c
	end
   else
	if b >= c then
		return b
	else
		return c
	end
   end
end

function main () 
	local a : number = 8
	local b : number = 6
	local c : number = 10
	local najvacsi : number = 0
	najvacsi = najvacsi_numb(a, b, c)
end

main()
