-- Definition of special functions for the Weapon object type.

require "resources/things/Thing"

Weapon = inheritsFrom(Thing, "Weapon")
Weapon.intrinsics.name = "[Weapon]"
Weapon.intrinsics.plural = "[Weapons]"

function Weapon.get_description()
	return "A thing designed or used for inflicting bodily harm or physical damage."
end