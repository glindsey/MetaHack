-- Definition of special functions for the LightSource object type.

require "resources/things/Thing"

LightSource = inheritsFrom(Thing, "LightSource")
LightSource.intrinsics.name = "[LightSource]"
LightSource.intrinsics.plural = "[LightSources]"

LightSource.intrinsics.light_lit = true
LightSource.intrinsics.light_color_red = 64
LightSource.intrinsics.light_color_green = 64
LightSource.intrinsics.light_color_blue = 64
LightSource.intrinsics.light_level = 256
LightSource.intrinsics.light_strength = 64

function LightSource.get_description()
    return "A source of illumination."
end


function LightSource.can_have_action_use_done_by(id)
    -- This SHOULD check if a creature is sentient.
    -- For now just return true.
    return true
end

function LightSource.be_object_of_action_use(id)
    local name = thing_get_intrinsic_string(id, "name")
    local is_lit = thing_get_property_flag(id, "light_lit")
    is_lit = not is_lit
    if is_lit then
        messageLog_add("You light the " .. name .. ".")
    else
        messageLog_add("You extinguish the " .. name .. ".")
    end
    thing_set_property_flag(id, "light_lit", is_lit)
    return ActionResult.Success
end