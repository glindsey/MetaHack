-- Definition of the Wall MapTile type.

require "resources/maptiles/MTUnknown"

MTWall = inheritsFrom(MTUnknown, "MTWall")
MTWall.intrinsics.name = "wall"

MTWall.intrinsics.passable = false
MTWall.intrinsics.opaque = true

function MTWall.get_description()
	return "A wall."
end