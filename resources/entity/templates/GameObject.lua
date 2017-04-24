-- Definition of the GameObject object type, Entity and MTUnknown inherit from.

GameObject = inheritsFrom(nil, "GameObject")

function GameObject_get_display_name(id)
    return thing_get_intrinsic(id, "name"), LuaType.String
end

function GameObject_get_brief_description(id)
    return "GameObject #" .. id .. ", which has no description associated with it.", LuaType.String
end

function GameObject_get_tile_offset(id, frame)
    return 0, 0, LuaType.IntVec2
end

function GameObject_on_create(id)
    --messageLog_add("GameObject.on_create() called, new ID = " .. id)
    return true, LuaType.Boolean
end