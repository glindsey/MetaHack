-- Definition of special functions for the LightSource object type.

require "resources/entity/Entity"

LightSource = inheritsFrom(Entity, "LightSource")
LightSource.intrinsics.name = "[LightSource]"
LightSource.intrinsics.plural = "[LightSources]"

LightSource.intrinsics.lit = true
LightSource.intrinsics.light_color = { type = "color", r = 64, g = 64, b = 64 }
LightSource.intrinsics.light_strength = 64

function LightSource.get_brief_description()
    return "A source of illumination.", LuaType.String
end

function LightSource.modify_property_attribute_strength(id, old_value)
    new_value = old_value + 1
    return new_value, LuaType.Integer
end

function LightSource.can_have_action_use_done_by(id)
    -- This SHOULD check if a creature is sentient.
    -- For now just return true.
    return true, LuaType.Boolean
end

function LightSource.be_object_of_action_use(object, subject)
    local name = thing_get_intrinsic(object, "name")
    local is_lit = thing_get_modified_property(object, "lit")
    is_lit = not is_lit
    if is_lit then
        messageLog_add("You light the " .. name .. ".")
        thing_add_property_modifier(subject, "attribute_strength", object)
    else
        messageLog_add("You extinguish the " .. name .. ".")
        thing_remove_property_modifier(subject, "attribute_strength", object)
    end
    thing_set_base_property(object, "lit", is_lit, LuaType.Boolean)
    return true, LuaType.Boolean
end