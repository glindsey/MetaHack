-- Definition of special functions for the FlaskGlass object type.

require "resources/entity/Container"

FlaskGlass = inheritsFrom(Container, "FlaskGlass")
FlaskGlass.intrinsics.name = "glass flask"
FlaskGlass.intrinsics.plural = "glass flasks"

FlaskGlass.intrinsics.inventory-size = 1
FlaskGlass.intrinsics.opacity = { type = "color", r = 0, g = 0, b = 0 }


function FlaskGlass.can_contain(id)
    -- A flask can contain both solids and liquids, so always return success.
    return true, LuaType.Boolean
end

function FlaskGlass.can_have_action_quaff_done_by(id)
    return true, LuaType.Boolean
end

function FlaskGlass.get_brief_description()
    return "A glass container with a narrow mouth and flat bottom, suitable for holding liquids.", LuaType.String
end
