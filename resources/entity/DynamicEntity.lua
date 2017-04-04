-- Definition of special functions for the DynamicEntity object type.

require "resources/entity/Entity"

DynamicEntity = inheritsFrom(Entity, "DynamicEntity")

function DynamicEntity.get_tile_offset(id, frame)
    -- If entity's hit points are <= 0, show the "dead" tile.
    local hp = thing_get_modified_property(id, "hp")
    if (hp > 0) then
        return 0, 0, LuaType.IntVec2
    else
        return 1, 0, LuaType.IntVec2
    end
end