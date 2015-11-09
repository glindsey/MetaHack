-- Definition of special functions for the Entity object type.

require "resources/things/Thing"

Entity = inheritsFrom(Thing, "Entity")
Entity.name = "[Entity]"

Entity.intrinsics.attributes = {}
Entity.intrinsics.bodypart.body.count = 1
Entity.intrinsics.opaque = true
Entity.intrinsics.living = true
Entity.intrinsics.inventory_size = -1

Entity.defaults.xp = 0

function Entity.get_tile_offset(id, frame)
	-- TODO: If entity's hit points are <= 0, show the "dead" tile.
	return 0, 0
end