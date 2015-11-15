-- Definition of special functions for the Chest object type.

require "resources/things/Container"

Chest = inheritsFrom(Container, "Chest")
Chest.intrinsics.name = "[Chest]"
Chest.intrinsics.plural = "[Chests]"

Chest.intrinsics.inventory_size = 20
Chest.intrinsics.lockable = true
Chest.intrinsics.physical_mass = 10

function Chest.get_description()
	return "A large strong box, typically made of wood and used for storage or shipping."
end