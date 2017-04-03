-- Definition of the GameObject object type, Entity and MTUnknown inherit from.

GameObject = inheritsFrom(nil, "GameObject")
GameObject.intrinsics = {}
GameObject.intrinsics.name = "[GameObject]"
GameObject.intrinsics.plural = "[GameObjects]"

GameObject.intrinsics.inventory_size = 0
GameObject.intrinsics.living = false
GameObject.intrinsics.passable = false
GameObject.intrinsics.opacity = { type = "color", r = 255, g = 255, b = 255 }
GameObject.intrinsics.tile_character_code = 0x003f   -- question mark
GameObject.intrinsics.tile_bg_color = { type = "color", r = 0, g = 0, b = 0 } -- black
GameObject.intrinsics.tile_fg_color = { type = "color", r = 32, g = 32, b = 32 } -- dark gray

GameObject.intrinsics.inventory_size_is_integer = true


function GameObject.get_display_name(id)
    return intrinsics.name
end

function GameObject.get_brief_description(id)
    return "GameObject #" .. id .. ", which has no description associated with it."
end

function GameObject.get_tile_offset(id, frame)
    return 0, 0, LuaType.IntVec2
end

function GameObject:get_tile_character_code(id, frame)
    return intrinsics.tile_character_code
end

function GameObject.get_tile_bg_color(id, frame)
    return intrinsics.tile_bg_color.r,
           intrinsics.tile_bg_color.g,
           intrinsics.tile_bg_color.b,
           255,
           LuaType.Color
end

function GameObject.get_tile_fg_color(id, frame)
    return intrinsics.tile_fg_color.r,
           intrinsics.tile_fg_color.g,
           intrinsics.tile_fg_color.b,
           255,
           LuaType.Color
end

function GameObject.on_create(id)
    --messageLog_add("GameObject.on_create() called, new ID = " .. id)
    return true, LuaType.Boolean
end