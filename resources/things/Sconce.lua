-- Definition of special functions for the Sconce object type.

require "resources/things/LightSource"

Sconce = inheritsFrom(LightSource, "Sconce")
Sconce.name = "torch sconce"

Sconce.defaults.light_color = color(160, 128, 112, 255)
Sconce.defaults.light_level = 256;
Sconce.defaults.light_strength = 48;

function Sconce.get_description()
	return "A flaming torch secured in a holder that is attached to a wall."
end

function Sconce.get_tile_offset(id, frame)
	local is_lit = thing_get_property_flag(id, "light_lit")
	if is_lit then
		return (math.floor(frame / 6) % 3) + 1, 0
    else
		return 0, 0
    end
end
