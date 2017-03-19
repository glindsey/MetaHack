-- Definition of special functions for the Blob object type.

require "resources/entity/DynamicEntity"

Blob = inheritsFrom(DynamicEntity, "Blob")
Blob.intrinsics.name = "[Blob]"
Blob.intrinsics.plural = "[Blobs]"

function Blob.get_brief_description()
	return "An amorphous entity with no distinguishable limbs, head, or other extremities.", PropertyType.String
end