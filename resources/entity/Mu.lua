-- Definition of special functions for the Mu (nothingness) object type.

require "resources/entity/Entity"

Mu = inheritsFrom(Entity, "Mu")
Mu.intrinsics.name = "[Mu]"
Mu.intrinsics.plural = "[Mu]"

Mu.intrinsics.opacity = { type = "color", r = 0, g = 0, b = 0 }

function Mu.get_brief_description()
	return "A completely empty space; the absence of all entities.", LuaType.String
end