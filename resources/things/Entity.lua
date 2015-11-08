-- Definition of special functions for the Entity object type.

require "resources/things/Thing"

Entity = inheritsFrom(Thing)

function Entity.get_tile_offset(id, frame)
	-- TODO: If entity's hit points are <= 0, show the "dead" tile.
	return 0, 0
end