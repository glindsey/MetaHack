-- Definition of the Floor MapTile type.

require "resources/maptile/MTUnknown"

MTFloor = inheritsFrom(MTUnknown, "MTFloor")
MTFloor.intrinsics.name = "floor"

MTFloor.intrinsics.passable = true
MTFloor.intrinsics.opacity_red = 0;
MTFloor.intrinsics.opacity_green = 0;
MTFloor.intrinsics.opacity_blue = 0;

function MTFloor.get_brief_description()
	return "A floor.", PropertyType.String
end