-- Definition of the Entity object type, which all others inherit from.

Entity = inheritsFrom(nil, "Entity")
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

function Entity.get_display_name(id)
    local quantity = thing_get_modified_property_value(id, "quantity")
    if quantity > 1 then
        return intrinsics.plural
    else
        return intrinsics.name
    end
end

function Entity.get_brief_description(id)
    return "Entity #" .. id .. ", which has no description associated with it."
end

function Entity.can_contain(id)
    -- By default a Entity can only contain solid objects.
    if thing_get_intrinsic_flag(id, "liquid") == true then
        return ActionResult.Failure
    end

    return ActionResult.Success
end

function Entity.can_have_action_drop_done_by(id)
    return true
end

function Entity.can_have_action_get_done_by(id)
    return true
end

function Entity.can_have_action_hurl_done_by(id)
    return true
end

function Entity.can_have_action_move_done_by(id)
    return true
end

function Entity.can_have_action_putinto_done_by(id)
    return true
end

function Entity.can_have_action_quaff_done_by(id)
    return false
end

function Entity.can_have_action_takeout_done_by(id)
    return true
end

function Entity.can_have_action_unwield_done_by(id)
    return true
end

function Entity.can_have_action_wield_done_by(id)
    return true
end

function Entity.get_tile_offset(id, frame)
    return 0, 0
end

function Entity.on_create(id)
    --messageLog_add("Entity.on_create() called, new ID = " .. id)
    return ActionResult.Success
end

function Entity.on_lit_by(id)
    return ActionResult.Success
end

function Entity.process()
    return ActionResult.Success
end

function Entity:get_intrinsic(key)
    local result = self.intrinsics[key]

    if (result == nil) then
        local superclass = self.superClass()

        if (superclass ~= nil) then
            return self.superClass():get_intrinsic(key)
        end
    else
        -- Look for special cases.
        if (type(result) == "table") then
            if (result.type == "range") then
                --print("Range type found, returning " .. tostring(result.min) .. " and " .. tostring(result.max))
                return math.random(result.min, result.max)
            elseif (result.type == "vector2") then
                return result.x, result.y
            end
        end

        -- Got here? Just return the result.
        return result
    end
end

function Entity:set_intrinsic(name, value)
    self.intrinsics[name] = value
end

