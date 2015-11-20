-- Definition of special functions for the WeaponStriking object type.

require "resources/things/Weapon"

WeaponStriking = inheritsFrom(Weapon, "WeaponStriking")
WeaponStriking.intrinsics.name = "[WeaponStriking]"
WeaponStriking.intrinsics.plural = "[WeaponStriking]"

function WeaponStriking.get_description()
	return "A weapon that is designed to inflict striking damage."
end