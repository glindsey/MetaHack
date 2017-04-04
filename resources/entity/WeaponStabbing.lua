-- Definition of special functions for the WeaponStabbing object type.

require "resources/entity/Weapon"

WeaponStabbing = inheritsFrom(Weapon, "WeaponStabbing")
WeaponStabbing.intrinsics.name = "[WeaponStabbing]"
WeaponStabbing.intrinsics.plural = "[WeaponsStabbing]"

function WeaponStabbing.get_brief_description()
	return "A weapon that is designed to inflict stabbing damage.", LuaType.String
end