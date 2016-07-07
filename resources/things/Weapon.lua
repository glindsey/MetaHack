-- Definition of special functions for the Weapon object type.

require "resources/things/Thing"

Weapon = inheritsFrom(Thing, "Weapon")
Weapon.intrinsics.name = "[Weapon]"
Weapon.intrinsics.plural = "[Weapons]"

function Weapon.get_brief_description()
    return "A thing designed or used for inflicting bodily harm or physical damage."
end

function Weapon.be_object_of_action_wield(object, subject)   
    print("*** Got call to be_object_of_action_wield(" .. object .. ", " .. subject .. ")")
    return ActionResult.Success
end 