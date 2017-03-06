-- Definition of special functions for the Coin object type.

require "resources/entity/Entity"

Coin = inheritsFrom(Entity, "Coin")
Coin.intrinsics.name = "[Coin]"
Coin.intrinsics.plural = "[Coins]"

Coin.intrinsics.physical_mass = 1

function Coin.get_brief_description()
    return "A flat, typically round piece of metal with an official stamp, used as money."
end

function Coin.get_tile_offset(id, frame)
    local quantity = thing_get_base_property_value(id, "quantity")
    if (quantity < 10) then
        return 0, 0
    elseif (quantity < 100) then
        return 1, 0
    else
        return 2, 0
    end
end