-- Definition of special functions for the WeaponStriking object type.

require "resources/entity/Weapon"

WeaponStriking = inheritsFrom(Weapon, "WeaponStriking")
WeaponStriking.intrinsics.name = "[WeaponStriking]"
WeaponStriking.intrinsics.plural = "[WeaponStriking]"

function WeaponStriking.get_brief_description()
	return "A weapon that is designed to inflict striking damage.", LuaType.String
end