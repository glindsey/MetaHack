-- Definition of special functions for the SackCloth object type.

require "resources/things/Container"

SackCloth = inheritsFrom(Container, "SackCloth")
SackCloth.intrinsics.name = "cloth sack"
SackCloth.intrinsics.plural = "cloth sacks"
SackCloth.intrinsics.creatable = true

SackCloth.intrinsics.inventory_size = 10

SackCloth.intrinsics.open = true

function Container.can_have_action_open_done_by(id)
    return false
end

function Container.can_have_action_close_done_by(id)
    return false
end

function SackCloth.get_description()
    return "A sack made out of cloth."
end
