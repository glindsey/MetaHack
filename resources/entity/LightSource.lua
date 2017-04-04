-- Definition of special functions for the LightSource object type.

require "resources/entity/Entity"

LightSource = inheritsFrom(Entity, "LightSource")

function LightSource.modify_property_attribute_strength(id, old_value)
    new_value = old_value + 1
    return new_value, LuaType.Integer
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
