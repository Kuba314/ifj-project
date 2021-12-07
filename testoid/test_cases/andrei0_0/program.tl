require "ifj21"

function main1 (a : integer, b : number, str : string,  c : integer, d : number)
    local id : integer = a + c + 12 + 34
    local id1 : number = b + d - 90 + 78 * 54
    local id2 : string = str.."string11111"
	write(id, id1, id2)
end

main1(12, 13.5, "andr", 324, 90.899)
