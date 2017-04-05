-- Definition of special functions for the Weapon object type.

function Weapon_be_object_of_action_wield(object, subject)   
    print("*** Got call to be_object_of_action_wield(" .. object .. ", " .. subject .. ")")
    return true, LuaType.Boolean
end 