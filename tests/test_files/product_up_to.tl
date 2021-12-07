require "ifj21"

function product_up_to(n: integer): integer
    local product: integer = 1

    for i = 1, n, 1 do
        product = product * i
    end

    return product
end

write("7! == ", product_up_to(7), "\n")
