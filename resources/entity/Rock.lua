-- Definition of special functions for the Rock object type.

require "resources/entity/Entity"

Rock = inheritsFrom(Entity, "Rock")
Rock.intrinsics.name = "rock"
Rock.intrinsics.plural = "rocks"
Rock.intrinsics.creatable = true

Rock.intrinsics.physical_mass = 3

function Rock.get_brief_description()
	return "A stone of any size, especially one small enough to be picked up and used as a projectile."
end