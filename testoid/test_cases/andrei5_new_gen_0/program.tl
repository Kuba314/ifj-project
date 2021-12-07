require "ifj21"
global funcAndrei : function(integer    ) : integer

function funcAndrei (idx : integer) : integer
    local str1 : string = "STR1"
    local str2 : string = "concat"
    local str3 : string = "STR2"

    str1 = str2..str2..str3
    write(str1, "\n")
    idx = ord(str1, idx)

    if idx == 83 then
        write("ord works good!\n")
    else
        write("ord doesnt work good!\n")
    end

    return idx

end

function main(idx : integer)
    local i : number
    i = funcAndrei(idx)

    write(i, "\n")
end

main(1)
