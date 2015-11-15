-- Definition of special functions for the ChestWooden object type.

require "resources/things/Chest"

ChestWooden = inheritsFrom(Chest, "ChestWooden")
ChestWooden.intrinsics.name = "wooden chest"
ChestWooden.intrinsics.plural = "wooden chests"

ChestWooden.intrinsics.flammable = true

function ChestWooden.get_description()
	return "A wooden chest."
end