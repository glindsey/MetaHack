-- Definition of special functions for the Marcher object type.

require "resources/entity/Biped"

Marcher = inheritsFrom(Biped, "Marcher")
Marcher.intrinsics.name = "marcher robot"
Marcher.intrinsics.plural = "marcher robot"
Marcher.intrinsics.creatable = true

Marcher.intrinsics.maxhp = range(1, 1)

function Marcher.get_brief_description()
    return "A testing DynamicEntity that endlessly marches left and right.", LuaType.String
end

function Marcher.process(id)
    print("DEBUG: Marcher.process() called for object " .. id .. ".")
    return true, LuaType.Boolean
end