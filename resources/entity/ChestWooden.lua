-- Definition of special functions for the ChestWooden object type.

require "resources/entity/Chest"

ChestWooden = inheritsFrom(Chest, "ChestWooden")
ChestWooden.intrinsics.name = "wooden chest"
ChestWooden.intrinsics.plural = "wooden chests"
ChestWooden.intrinsics.creatable = true

function ChestWooden.can_have_action_burn_done_by(id)
    return true, LuaType.Boolean
end

function ChestWooden.get_brief_description()
    return "A wooden chest.", LuaType.String
end