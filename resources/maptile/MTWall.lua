-- Definition of the Wall MapTile type.

require "resources/maptile/MTUnknown"

MTWall = inheritsFrom(MTUnknown, "MTWall")
MTWall.intrinsics.name = "wall"

MTWall.intrinsics.passable = false
MTWall.intrinsics.opacity_red = 255
MTWall.intrinsics.opacity_green = 255
MTWall.intrinsics.opacity_blue = 255

function MTWall.get_brief_description()
	return "A wall."
end