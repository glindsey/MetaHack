-- Definition of special functions for the Blob object type.

require "resources/things/Entity"

Blob = inheritsFrom(Entity, "Blob")
Blob.name = "[Blob]"

function Blob.get_description()
	return "An amorphous entity with no distinguishable limbs, head, or other extremities."
end