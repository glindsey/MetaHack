-- Definition of special functions for the Sapient object type.

require "resources/entity/Animal"

Sapient = inheritsFrom(Animal, "Sapient")
Sapient.intrinsics.name = "[Sapient]"
Sapient.intrinsics.plural = "[Sapients]"

Sapient.intrinsics.can_close = true
Sapient.intrinsics.can_disrobe = true
Sapient.intrinsics.can_lock = true
Sapient.intrinsics.can_mix = true
Sapient.intrinsics.can_open = true
Sapient.intrinsics.can_shoot = true
Sapient.intrinsics.can_talk = true
Sapient.intrinsics.can_unlock = true
Sapient.intrinsics.can_unwield = true
Sapient.intrinsics.can_use = true
Sapient.intrinsics.can_wear = true
Sapient.intrinsics.can_wield = true


function Sapient.get_brief_description()
	return "An animal that is capable of free will and independent thought."
end