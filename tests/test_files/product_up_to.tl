require "ifj21"

function product_up_to(n: integer): integer
    local product: integer

    for i = integer, i > 0, 1 do
        product = product * i
    end

    return product
end
