-- Definition of special functions for the Sconce object type.

require "resources/things/LightSource"

Sconce = inheritsFrom(LightSource, "Sconce")

function Sconce.get_tile_offset(id, frame)
	local is_lit = thing_get_property_flag(id, "light_lit")
	if is_lit then
		return (math.floor(frame / 6) % 3) + 1, 0
    else
		return 0, 0
    end
end
