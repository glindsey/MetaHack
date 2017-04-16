-- Definition of special functions for the Coin object type.

function Coin_get_tile_offset(id, frame)
    local quantity = get_quantity(id)
    if (quantity < 10) then
        return 0, 0, LuaType.IntVec2
    elseif (quantity < 100) then
        return 1, 0, LuaType.IntVec2
    else
        return 2, 0, LuaType.IntVec2
    end
end