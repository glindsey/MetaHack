-- Definition of special functions for the Floor object type.

require "resources/things/Thing"

Floor = inheritsFrom(Thing, "Floor")
Floor.intrinsics.name = "floor"
Floor.intrinsics.plural = "floors"

Floor.intrinsics.movable = false
Floor.intrinsics.opaque = false
Floor.intrinsics.inventory_size = -1

function Floor.get_description()
	return "The lower surface of a room, on which one may walk."
end