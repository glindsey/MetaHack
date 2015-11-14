-- Definition of special functions for the Container object type.

require "resources/things/Thing"

Container = inheritsFrom(Thing, "Container")
Container.intrinsics.name = "[Container]"
Container.intrinsics.plural = "[Containers]"

Container.intrinsics.openable = true
Container.intrinsics.inventory_size = -1

function Container.get_description()
	return "An object that can be used to hold or transport something."
end
