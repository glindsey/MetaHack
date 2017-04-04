-- Definition of special functions for the WeaponStrangling object type.

require "resources/entity/Weapon"

WeaponStrangling = inheritsFrom(Weapon, "WeaponStrangling")
WeaponStrangling.intrinsics.name = "[WeaponStrangling]"
WeaponStrangling.intrinsics.plural = "[WeaponStrangling]"

function WeaponStrangling.get_brief_description()
	return "A weapon that is designed to inflict strangling damage.", LuaType.String
end