-- Definition of special functions for the Marcher object type.

require "resources/entity/Biped"

Marcher = inheritsFrom(Biped, "Marcher")

function Marcher.process(id)
    print("DEBUG: Marcher.process() called for object " .. id .. ".")
    return true, LuaType.Boolean
end