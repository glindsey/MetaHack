-- Definition of special functions for the DynamicEntity object type.

require "resources/entity/Entity"

DynamicEntity = inheritsFrom(Entity, "DynamicEntity")
DynamicEntity.intrinsics.name = "[DynamicEntity]"
DynamicEntity.intrinsics.plural = "[DynamicEntities]"

DynamicEntity.intrinsics.bodypart_body_count = 1
DynamicEntity.intrinsics.opacity = { type = "color", r = 255, g = 255, b = 255 }
DynamicEntity.intrinsics.living = true
DynamicEntity.intrinsics.inventory-size = -1

DynamicEntity.intrinsics.can_die = true


DynamicEntity.intrinsics.xp = 0

function DynamicEntity.can_have_action_attack_done_by(id)
    return true, LuaType.Boolean
end

function DynamicEntity.can_have_action_die_done_by(id)
    return true, LuaType.Boolean
end

function DynamicEntity.can_have_action_hurt_done_by(id)
    return true, LuaType.Boolean
end

function DynamicEntity.can_have_action_burn_done_by(id)
    return true, LuaType.Boolean
end

function DynamicEntity.get_tile_offset(id, frame)
    -- If entity's hit points are <= 0, show the "dead" tile.
    local hp = thing_get_modified_property(id, "hp")
    if (hp > 0) then
        return 0, 0, LuaType.IntVec2
    else
        return 1, 0, LuaType.IntVec2
    end
end