-- Definition of special functions for the ChestWooden object type.

require "resources/things/Container"

ChestWooden = inheritsFrom(Container, "ChestWooden")
ChestWooden.name = "wooden chest"

ChestWooden.intrinsics.lockable = true
ChestWooden.intrinsics.flammable = true
ChestWooden.inventory_size = 20
ChestWooden.physical_mass = 10

function ChestWooden.get_description
	return "A large strong box, typically made of wood and used for storage or shipping."
end