-- Definition of the FloorStone MapTile type.

require "resources/maptiles/MTFloor"

MTFloorStone = inheritsFrom(MTFloor, "MTFloorStone")
MTFloorStone.intrinsics.name = "stone floor"

function MTFloorStone.get_description()
	return "A stone floor."
end