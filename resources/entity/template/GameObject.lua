-- Definition of the GameObject object type, which everything else inherits from.

GameObject = inheritsFrom(nil, "GameObject")

function GameObject_get_display_name(id)
    return entity_get_intrinsic(id, "name"), LuaType.String
end

function GameObject_get_brief_description(id)
    return "GameObject #" .. id .. ", which has no description associated with it.", LuaType.String
end

function GameObject_get_tile_offset(id, frame)
    return 0, 0, LuaType.IntVec2
end

function GameObject_on_create(id)
    --message("GameObject.on_create() called, new ID = " .. id)
    return true, LuaType.Boolean
end