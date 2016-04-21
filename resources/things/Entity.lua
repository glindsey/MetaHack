-- Definition of special functions for the Entity object type.

require "resources/things/Thing"

Entity = inheritsFrom(Thing, "Entity")
Entity.intrinsics.name = "[Entity]"
Entity.intrinsics.plural = "[Entities]"

Entity.intrinsics.bodypart_body_count = 1
Entity.intrinsics.opacity_red = 255
Entity.intrinsics.opacity_green = 255
Entity.intrinsics.opacity_blue = 255
Entity.intrinsics.living = true
Entity.intrinsics.inventory_size = -1

Entity.intrinsics.xp = 0

function Entity.can_have_action_attack_done_by(id)
    return true
end

function Entity.can_have_action_die_done_by(id)
    return true
end

function Entity.can_have_action_hurt_done_by(id)
    return true
end

function Entity.get_tile_offset(id, frame)
    -- If entity's hit points are <= 0, show the "dead" tile.
    local hp = thing_get_modified_property_value(id, "hp")
    if (hp > 0) then
        return 0, 0
    else
        return 1, 0
    end
end