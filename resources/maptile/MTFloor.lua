-- Definition of the Floor MapTile type.

require "resources/maptile/MTUnknown"

MTFloor = inheritsFrom(MTUnknown, "MTFloor")
MTFloor.intrinsics.name = "floor"

MTFloor.intrinsics.passable = true
MTFloor.intrinsics.opacity = { type = "color", r = 0, g = 0, b = 0 }

function MTFloor.get_brief_description()
	return "A floor.", LuaType.String
end