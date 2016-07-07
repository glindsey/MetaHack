-- Definition of special functions for the Blob object type.

require "resources/things/Entity"

Blob = inheritsFrom(Entity, "Blob")
Blob.intrinsics.name = "[Blob]"
Blob.intrinsics.plural = "[Blobs]"

function Blob.get_brief_description()
	return "An amorphous entity with no distinguishable limbs, head, or other extremities."
end