-- Definition of special functions for the Weapon object type.

function Weapon_on_object_of_wield(object, subject)   
    print("*** Got call to Weapon_on_object_of_wield(" .. object .. ", " .. subject .. ")")
    return true, LuaType.Boolean
end 