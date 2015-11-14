-- Definition of the Floor MapTile type.

require "resources/maptiles/MTUnknown"

MTFloor = inheritsFrom(MTUnknown, "MTFloor")
MTFloor.intrinsics.name = "floor"

MTFloor.intrinsics.passable = true
MTFloor.intrinsics.opaque = false

function MTFloor.get_description()
	return "A floor."
end