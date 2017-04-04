-- Definition of special functions for the Weapon object type.

require "resources/entity/Entity"

Weapon = inheritsFrom(Entity, "Weapon")

function Weapon.be_object_of_action_wield(object, subject)   
    print("*** Got call to be_object_of_action_wield(" .. object .. ", " .. subject .. ")")
    return true, LuaType.Boolean
end 