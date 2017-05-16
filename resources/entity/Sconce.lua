-- Definition of special functions for the Sconce object type.

function Sconce_get_tile_offset(id, frame)
    local lit = is_lit(id)
    if lit then
        return (math.floor(frame / 6) % 3) + 1, 0, LuaType.IntVec2
    else
        return 0, 0, LuaType.IntVec2
    end
end
