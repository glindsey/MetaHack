-- Definition of special functions for the Sapient object type.

require "resources/things/Animal"

Sapient = inheritsFrom(Animal, "Sapient")
Sapient.intrinsics.name = "[Sapient]"
Sapient.intrinsics.plural = "[Sapients]"

Sapient.intrinsics.can_talk = true

function Sapient.get_brief_description()
	return "An animal that is capable of free will and independent thought."
end