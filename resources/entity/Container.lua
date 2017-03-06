-- Definition of special functions for the Container object type.

require "resources/entity/Entity"

Container = inheritsFrom(Entity, "Container")
Container.intrinsics.name = "[Container]"
Container.intrinsics.plural = "[Containers]"

Container.intrinsics.inventory_size = -1

function Container.can_have_action_open_done_by(id)
    return true
end

function Container.can_have_action_close_done_by(id)
    return true
end

function Container.get_brief_description()
    return "An object that can be used to hold or transport something."
end
