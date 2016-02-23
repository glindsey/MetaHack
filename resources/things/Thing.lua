-- Definition of the Thing object type, which all others inherit from.

Thing = inheritsFrom(nil, "Thing")
Thing.intrinsics.name = "[Thing]"
Thing.intrinsics.plural = "[Things]"

Thing.intrinsics.bodypart_body_name = "body"
Thing.intrinsics.bodypart_body_plural = "bodies"
Thing.intrinsics.bodypart_skin_name = "skin"
Thing.intrinsics.bodypart_head_name = "head"
Thing.intrinsics.bodypart_ear_name = "ear"
Thing.intrinsics.bodypart_eye_name = "eye"
Thing.intrinsics.bodypart_nose_name = "nose"
Thing.intrinsics.bodypart_mouth_name = "mouth"
Thing.intrinsics.bodypart_neck_name = "neck"
Thing.intrinsics.bodypart_chest_name = "chest"
Thing.intrinsics.bodypart_arm_name = "arm"
Thing.intrinsics.bodypart_hand_name = "hand"
Thing.intrinsics.bodypart_leg_name = "leg"
Thing.intrinsics.bodypart_foot_name = "foot"
Thing.intrinsics.bodypart_foot_plural = "feet"
Thing.intrinsics.bodypart_wing_name = "wing"
Thing.intrinsics.bodypart_tail_name = "tail"

Thing.intrinsics.inventory_size = 0
Thing.intrinsics.living = false
Thing.intrinsics.opacity_red = 255
Thing.intrinsics.opacity_green = 255
Thing.intrinsics.opacity_blue = 255
Thing.intrinsics.physical_mass = 0

Thing.defaults.counter_busy = 0
Thing.defaults.locked = false
Thing.defaults.open = true
Thing.defaults.quantity = 1

function Thing.get_description(id)
    return "Thing #" .. id .. ", which has no description associated with it."
end

function Thing.can_have_action_get_done_by(id)
    return true
end

function Thing.can_have_action_hurl_done_by(id)
    return true
end

function Thing.can_have_action_move_done_by(id)
    return true
end

function Thing.can_have_action_putinto_done_by(id)
    return true
end

function Thing.can_have_action_takeout_done_by(id)
    return true
end

function Thing.can_have_action_wield_done_by(id)
    return true
end

function Thing.can_contain(id)
    return ActionResult.Success
end

function Thing.get_tile_offset(id, frame)
    return 0, 0
end

function Thing.on_create(id)
    --messageLog_add("Thing.on_create() called, new ID = " .. id)
    return ActionResult.Success
end

function Thing.on_lit_by(id)
    return ActionResult.Success
end

function Thing.process()
    return ActionResult.Success
end

function Thing:get_intrinsic(name)
    local result = self.intrinsics[name]
    
    if (result == nil) then
        local superclass = self.superClass()
        
        if (superclass ~= nil) then
            return self.superClass():get_intrinsic(name)
        else
            --error("Traversed all the way to root class but could not find " .. name)
        end
    else
        -- Look for special cases.
        if (type(result) == "table") then
            if (result.type == "range") then
                --print("Range type found, returning " .. tostring(result.min) .. " and " .. tostring(result.max))
                return result.min, result.max
            elseif (result.type == "vector2") then
                return result.x, result.y
            end
        end

        -- Got here? Just return the result.
        return result
    end
end

function Thing:get_default(name)
    local result = self.defaults[name]
    
    if (self.type == "LightOrb" or self.type == "LightSource") then
        print("Asked thing " .. self.type .. " for default " .. name .. " and got result " .. tostring(result))
    end
    
    if (result == nil) then
        local superclass = self.superClass()
        
        if (superclass ~= nil) then
            return self.superClass():get_default(name)
        else
            --error("Traversed all the way to root class but could not find " .. name)
        end
    else
        -- Look for special cases.
        if (type(result) == "table") then
            if (result.type == "range") then
                --print("Range type found, returning " .. tostring(result.min) .. " and " .. tostring(result.max))
                return result.min, result.max
            elseif (result.type == "vector2") then
                return result.x, result.y
            end
        end

        -- Got here? Just return the result.
        return result
    end		
end

function Thing:set_intrinsic(name, value)
    self.intrinsics[name] = value
end

