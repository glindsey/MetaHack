-- Definition of the LightOrb object type.

require "resources/things/LightSource"

LightOrb = inheritsFrom(LightSource, "Light Orb")
LightOrb.name = "magical lighting orb"

LightOrb.defaults.physical_mass = 2

function LightOrb.get_description()
	return "A glowing orb, used for testing."
end