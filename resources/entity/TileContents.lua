-- Definition of special functions for the TileContents object type.

require "resources/entity/Entity"

TileContents = inheritsFrom(Entity, "TileContents")
TileContents.intrinsics.name = "area"
TileContents.intrinsics.plural = "areas"

TileContents.intrinsics.opacity = { type = "color", r = 0, g = 0, b = 0 }
TileContents.intrinsics.inventory_size = -1

function TileContents.get_brief_description()
	return "The entities that are held or included in this tile.", LuaType.String
end