-- Definition of the Entity object type, which all others inherit from.

Entity = inheritsFrom(GameObject, "Entity")
Entity.intrinsics.name = "[Entity]"
Entity.intrinsics.plural = "[Entities]"

Entity.intrinsics.bodypart_body_name = "body"
Entity.intrinsics.bodypart_body_plural = "bodies"
Entity.intrinsics.bodypart_skin_name = "skin"
Entity.intrinsics.bodypart_head_name = "head"
Entity.intrinsics.bodypart_ear_name = "ear"
Entity.intrinsics.bodypart_eye_name = "eye"
Entity.intrinsics.bodypart_nose_name = "nose"
Entity.intrinsics.bodypart_mouth_name = "mouth"
Entity.intrinsics.bodypart_neck_name = "neck"
Entity.intrinsics.bodypart_chest_name = "chest"
Entity.intrinsics.bodypart_arm_name = "arm"
Entity.intrinsics.bodypart_hand_name = "hand"
Entity.intrinsics.bodypart_leg_name = "leg"
Entity.intrinsics.bodypart_foot_name = "foot"
Entity.intrinsics.bodypart_foot_plural = "feet"
Entity.intrinsics.bodypart_wing_name = "wing"
Entity.intrinsics.bodypart_tail_name = "tail"

Entity.intrinsics.inventory_size = 0
Entity.intrinsics.living = false
Entity.intrinsics.opacity_red = 255
Entity.intrinsics.opacity_green = 255
Entity.intrinsics.opacity_blue = 255
Entity.intrinsics.physical_mass = 0

Entity.intrinsics.counter_busy = 0
Entity.intrinsics.locked = false
Entity.intrinsics.open = true
Entity.intrinsics.quantity = 1

Entity.intrinsics.inventory_size_is_integer = true
Entity.intrinsics.physical_mass_is_integer = true
Entity.intrinsics.counter_busy_is_integer = true
Entity.intrinsics.quantity_is_integer = true


function Entity.get_display_name(id)
    local quantity = thing_get_modified_property(id, "quantity")
    if quantity > 1 then
        return intrinsics.plural, PropertyType.String
    else
        return intrinsics.name, PropertyType.String
    end
end

function Entity.get_brief_description(id)
    return "Entity #" .. id .. ", which has no description associated with it.", PropertyType.String
end

function Entity.can_contain(id)
    -- By default a Entity can only contain solid objects.
    if thing_get_intrinsic(id, "liquid") == true then
        return ActionResult.Failure, PropertyType.ActionResult
    end

    return ActionResult.Success, PropertyType.ActionResult
end

function Entity.can_have_action_drop_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_get_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_hurl_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_move_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_putinto_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_quaff_done_by(id)
    return false, ActionResult.Boolean
end

function Entity.can_have_action_takeout_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_unwield_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.can_have_action_wield_done_by(id)
    return true, ActionResult.Boolean
end

function Entity.get_tile_offset(id, frame)
    return 0, 0, PropertyType.IntVec2
end

function Entity.get_tile_character_code(id, frame)
    return 0x003f, ActionResult.Integer           -- question mark
end

function Entity.get_tile_bg_color(id, frame)
    return 0, 0, 0, 255, ActionResult.Color     -- black
end

function Entity.get_tile_fg_color(id, frame)
    return 255, 255, 0, 255, ActionResult.Color -- yellow
end

function Entity.on_create(id)
    --messageLog_add("Entity.on_create() called, new ID = " .. id)
    return ActionResult.Success, PropertyType.ActionResult
end

function Entity.on_lit_by(id)
    return ActionResult.Success, PropertyType.ActionResult
end

function Entity.process()
    return ActionResult.Success, PropertyType.ActionResult
end