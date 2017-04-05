-- Definition of special functions for the FlaskGlass object type.

function FlaskGlass_can_contain(id)
    -- A flask can contain both solids and liquids, so always return success.
    return true, LuaType.Boolean
end
