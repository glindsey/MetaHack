-- Definition of special functions for the Entity object type.

Entity = Entity or {}

function Sconce.get_tile_offset(id, frame)
	-- TODO: If entity's hit points are <= 0, show the "dead" tile.
	return 0, 0
end