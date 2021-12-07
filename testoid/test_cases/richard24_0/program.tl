require "ifj21"
function main () 
	local sentence : string = "AndreiJeNajvacsiFrajerNaSkole"
	local word5 : string = substr(sentence, 0, 5)
	local word6 : string = substr(sentence, 6, 7)
	local word1 : string = substr(sentence, 8, 15)
	local word2 : string = substr(sentence, 16, 21)
	local word3 : string = substr(sentence, 22, 23)
	local word4 : string = substr(sentence, 24, 28)
	write(word1, word2, word3, word4, word5, word6) 
end

main()
