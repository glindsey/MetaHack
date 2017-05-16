-- Definition of special functions for the LightSource object type.

function LightSource_modify_property_attribute_strength(id, old_value)
    new_value = old_value + 1
    return new_value, LuaType.Integer
end

function LightSource_on_object_of_use(object, subject)
    local name = thing_get_intrinsic(object, "name")
    local lit = is_lit(object)
    lit = not lit
    set_lit(object, lit)
    return true, LuaType.Boolean
end
