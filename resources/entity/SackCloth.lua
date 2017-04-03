-- Definition of special functions for the SackCloth object type.

require "resources/entity/Container"

SackCloth = inheritsFrom(Container, "SackCloth")
SackCloth.intrinsics.name = "cloth sack"
SackCloth.intrinsics.plural = "cloth sacks"
SackCloth.intrinsics.creatable = true

SackCloth.intrinsics.inventory_size = 10

SackCloth.intrinsics.open = true

function SackCloth.can_have_action_open_done_by(id)
    return false, LuaType.Boolean
end

function SackCloth.can_have_action_close_done_by(id)
    return false, LuaType.Boolean
end

function SackCloth.can_have_action_burn_done_by(id)
    return true, LuaType.Boolean
end

function SackCloth.get_brief_description()
    return "A sack made out of cloth.", LuaType.String
end
