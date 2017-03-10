-- Definition of the LightOrb object type.

require "resources/entity/LightSource"

LightOrb = inheritsFrom(LightSource, "LightOrb")
LightOrb.intrinsics.name = "magical lighting orb"
LightOrb.intrinsics.plural = "magical lighting orbs"
LightOrb.intrinsics.creatable = true

LightOrb.intrinsics.light_color_red = 128
LightOrb.intrinsics.light_color_green = 128
LightOrb.intrinsics.light_color_blue = 255
LightOrb.intrinsics.light_strength = 128

LightOrb.intrinsics.physical_mass = 2

function LightOrb.get_brief_description()
	return "A glowing orb, used for testing."
end