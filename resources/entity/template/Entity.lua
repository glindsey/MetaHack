-- Definition of the Entity object type, which all others inherit from.

function Entity_get_display_name(id)
    local quantity = get_quantity(id)
    if quantity > 1 then
        return entity_get_intrinsic(id, "plural"), LuaType.String
    else
        return entity_get_intrinsic(id, "name"), LuaType.String
    end
end

function Entity_get_brief_description(id)
    return "Entity #" .. id .. ", which has no description associated with it.", LuaType.String
end

function Entity_can_contain(id)
    -- By default a Entity can only contain solid objects.
    if entity_get_intrinsic(id, "liquid") == true then
        return false, LuaType.Boolean
    end

    return true, LuaType.Boolean
end

function Entity_on_lit_by(id)
    return true, LuaType.Boolean
end

function Entity_process()
    return true, LuaType.Boolean
end

function Entity_get_tile_offset_DISABLED(id, frame)
    -- If entity's hit points are <= 0, show the "dead" tile.
    local hp = get_hp(id);
    if (hp > 0) then
        return 0, 0, LuaType.IntVec2
    else
        return 1, 0, LuaType.IntVec2
    end
end