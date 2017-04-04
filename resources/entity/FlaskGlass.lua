-- Definition of special functions for the FlaskGlass object type.

require "resources/entity/Container"

FlaskGlass = inheritsFrom(Container, "FlaskGlass")

function FlaskGlass.can_contain(id)
    -- A flask can contain both solids and liquids, so always return success.
    return true, LuaType.Boolean
end
