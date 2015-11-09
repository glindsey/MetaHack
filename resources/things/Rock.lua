-- Definition of special functions for the Rock object type.

require "resources/things/Thing"

Rock = inheritsFrom(Thing, "Rock")
Rock.name = "rock"

Rock.intrinsics.physical_mass = 3

function Rock.get_description()
	return "A stone of any size, especially one small enough to be picked up and used as a projectile."
end