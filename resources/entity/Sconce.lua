-- Definition of special functions for the Sconce object type.

require "resources/entity/LightSource"

Sconce = inheritsFrom(LightSource, "Sconce")

function Sconce.get_tile_offset(id, frame)
    local is_lit = thing_get_modified_property(id, "lit")
    if is_lit then
        return (math.floor(frame / 6) % 3) + 1, 0, LuaType.IntVec2
    else
        return 0, 0, LuaType.IntVec2
    end
end
