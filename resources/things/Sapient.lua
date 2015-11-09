-- Definition of special functions for the Sapient object type.

require "resources/things/Animal"

Sapient = inheritsFrom(Animal, "Sapient")
Sapient.name = "[Sapient]"

Sapient.intrinsics.can_talk = true

function Sapient.get_description()
	return "An animal that is capable of free will and independent thought."
end