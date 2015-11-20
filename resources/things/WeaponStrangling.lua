-- Definition of special functions for the WeaponStrangling object type.

require "resources/things/Weapon"

WeaponStrangling = inheritsFrom(Weapon, "WeaponStrangling")
WeaponStrangling.intrinsics.name = "[WeaponStrangling]"
WeaponStrangling.intrinsics.plural = "[WeaponStrangling]"

function WeaponStrangling.get_description()
	return "A weapon that is designed to inflict strangling damage."
end