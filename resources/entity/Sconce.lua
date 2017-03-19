-- Definition of special functions for the Sconce object type.

require "resources/entity/LightSource"

Sconce = inheritsFrom(LightSource, "Sconce")
Sconce.intrinsics.name = "torch sconce"
Sconce.intrinsics.plural = "torch sconces"
Sconce.intrinsics.creatable = true

Sconce.intrinsics.light_color_red = 160
Sconce.intrinsics.light_color_green = 128
Sconce.intrinsics.light_color_blue = 112
Sconce.intrinsics.light_strength = 48

function Sconce.can_have_action_burn_done_by(id)
    return true, PropertyType.Boolean
end

function Sconce.get_brief_description()
    return "A flaming torch secured in a holder that is attached to a wall.", PropertyType.String
end

function Sconce.get_tile_offset(id, frame)
    local is_lit = thing_get_modified_property(id, "lit")
    if is_lit then
        return (math.floor(frame / 6) % 3) + 1, 0, PropertyType.IntVec2
    else
        return 0, 0, PropertyType.IntVec2
    end
end
