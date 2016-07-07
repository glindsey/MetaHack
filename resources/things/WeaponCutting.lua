-- Definition of special functions for the WeaponCutting object type.

require "resources/things/Weapon"

WeaponCutting = inheritsFrom(Weapon, "WeaponCutting")
WeaponCutting.intrinsics.name = "[WeaponCutting]"
WeaponCutting.intrinsics.plural = "[WeaponCutting]"

function WeaponCutting.get_brief_description()
	return "A weapon that is designed to inflict cutting damage."
end