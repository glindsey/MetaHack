-- Definition of the WallStone MapTile type.

require "resources/maptile/MTWall"

MTWallStone = inheritsFrom(MTWall, "MTWallStone")
MTWallStone.intrinsics.name = "stone wall"

function MTWallStone.get_brief_description()
	return "A stone wall.", LuaType.String
end