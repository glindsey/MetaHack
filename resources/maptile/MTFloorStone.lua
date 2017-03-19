-- Definition of the FloorStone MapTile type.

require "resources/maptile/MTFloor"

MTFloorStone = inheritsFrom(MTFloor, "MTFloorStone")
MTFloorStone.intrinsics.name = "stone floor"

function MTFloorStone.get_brief_description()
	return "A stone floor.", PropertyType.String
end