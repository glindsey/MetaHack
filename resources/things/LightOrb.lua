-- Definition of the LightOrb object type.

require "resources/things/LightSource"

LightOrb = inheritsFrom(LightSource, "LightOrb")
LightOrb.intrinsics.name = "magical lighting orb"
LightOrb.intrinsics.plural = "magical lighting orbs"

LightOrb.defaults.physical_mass = 2

function LightOrb.get_description()
	return "A glowing orb, used for testing."
end