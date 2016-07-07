-- Definition of the WallStone MapTile type.

require "resources/maptiles/MTWall"

MTWallStone = inheritsFrom(MTWall, "MTWallStone")
MTWallStone.intrinsics.name = "stone wall"

function MTWallStone.get_brief_description()
	return "A stone wall."
end