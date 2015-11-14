-- Definition of the FloorDirt MapTile type.

require "resources/maptiles/MTFloor"

MTFloorDirt = inheritsFrom(MTFloor, "MTFloorDirt")
MTFloorDirt.intrinsics.name = "dirt floor"

function MTFloorDirt.get_description()
	return "A dirt floor."
end