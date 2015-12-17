-- Definition of special functions for the TileContents object type.

require "resources/things/Thing"

TileContents = inheritsFrom(Thing, "TileContents")
TileContents.intrinsics.name = "area"
TileContents.intrinsics.plural = "areas"

TileContents.intrinsics.movable = false
TileContents.intrinsics.opaque = false
TileContents.intrinsics.inventory_size = -1

function TileContents.get_description()
	return "The things that are held or included in this tile."
end