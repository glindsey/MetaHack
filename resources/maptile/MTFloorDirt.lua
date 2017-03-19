-- Definition of the FloorDirt MapTile type.

require "resources/maptile/MTFloor"

MTFloorDirt = inheritsFrom(MTFloor, "MTFloorDirt")
MTFloorDirt.intrinsics.name = "dirt floor"

function MTFloorDirt.get_brief_description()
	return "A dirt floor.", PropertyType.String
end