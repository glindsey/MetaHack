-- Definition of the Unknown MapTile type.

MTUnknown = inheritsFrom(GameObject, "MTUnknown")
MTUnknown.intrinsics.name = "unknown"

MTUnknown.intrinsics = MTUnknown.intrinsics or {}

MTUnknown.intrinsics.passable = false
MTUnknown.intrinsics.opacity = { type = "color", r = 255, g = 255, b = 255 }

MTUnknown.intrinsics.tile_character_code = 0x003f   -- question mark
MTUnknown.intrinsics.tile_bg_color = { type = "color", r = 0, g = 0, b = 0 } -- black
MTUnknown.intrinsics.tile_fg_color = { type = "color", r = 128, g = 64, b = 0 } -- brown

function MTUnknown.get_brief_description()
	return "Unexplored space.", LuaType.String
end