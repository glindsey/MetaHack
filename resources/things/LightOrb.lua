-- Definition of the LightOrb object type.

require "resources/things/LightSource"

LightOrb = inheritsFrom(LightSource, "LightOrb")
LightOrb.intrinsics.name = "magical lighting orb"
LightOrb.intrinsics.plural = "magical lighting orbs"
LightOrb.intrinsics.creatable = true

LightOrb.defaults.light_color_red = 255
LightOrb.defaults.light_color_green = 255
LightOrb.defaults.light_color_blue = 255
LightOrb.defaults.light_level = 256
LightOrb.defaults.light_strength = 255

LightOrb.defaults.physical_mass = 2

function LightOrb.get_description()
	return "A glowing orb, used for testing."
end