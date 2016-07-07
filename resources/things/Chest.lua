-- Definition of special functions for the Chest object type.

require "resources/things/Container"

Chest = inheritsFrom(Container, "Chest")
Chest.intrinsics.name = "[Chest]"
Chest.intrinsics.plural = "[Chests]"

Chest.intrinsics.inventory_size = 20
Chest.intrinsics.physical_mass = 10

function Chest.can_have_action_lock_done_by(id)
    return true
end

function Chest.can_have_action_unlock_done_by(id)
    return true
end

function Chest.get_brief_description()
    return "A large strong box, typically made of wood and used for storage or shipping."
end