-- Definition of the LightOrb object type.

require "resources/entity/LightSource"

LightOrb = inheritsFrom(LightSource, "LightOrb")
LightOrb.intrinsics.name = "magical lighting orb"
LightOrb.intrinsics.plural = "magical lighting orbs"
LightOrb.intrinsics.creatable = true

LightOrb.intrinsics.light_color = { type = "color", r = 128, g = 128, b = 255 }
LightOrb.intrinsics.light_strength = 128

LightOrb.intrinsics.physical_mass = 2

function LightOrb.get_brief_description()
	return "A glowing orb, used for testing.", PropertyType.String
end