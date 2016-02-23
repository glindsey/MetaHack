-- Definition of special functions for the Mu (nothingness) object type.

require "resources/things/Thing"

Mu = inheritsFrom(Thing, "Mu")
Mu.intrinsics.name = "[Mu]"
Mu.intrinsics.plural = "[Mu]"

Mu.intrinsics.opacity_red = 0
Mu.intrinsics.opacity_green = 0
Mu.intrinsics.opacity_blue = 0

function Mu.get_description()
	return "A completely empty space; the absence of all things."
end