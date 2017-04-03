-- Definition of the Wall MapTile type.

require "resources/maptile/MTUnknown"

MTWall = inheritsFrom(MTUnknown, "MTWall")
MTWall.intrinsics.name = "wall"

MTWall.intrinsics.passable = false
MTWall.intrinsics.opacity = { type = "color", r = 255, g = 255, b = 255 }

function MTWall.get_brief_description()
	return "A wall.", LuaType.String
end