-- Definition of special functions for the Mu (nothingness) object type.

require "resources/things/Thing"

Mu = inheritsFrom(Thing, "Mu")
Mu.intrinsics.name = "[Mu]"
Mu.intrinsics.plural = "[Mu]"

Mu.intrinsics.lockable = false
Mu.intrinsics.movable = false
Mu.intrinsics.opaque = false
Mu.intrinsics.openable = false

function Mu.get_description()
	return "A completely empty space; the absence of all things."
end