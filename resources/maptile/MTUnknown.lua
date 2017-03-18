-- Definition of the Unknown MapTile type.

MTUnknown = inheritsFrom(GameObject, "MTUnknown")
MTUnknown.intrinsics.name = "unknown"

MTUnknown.intrinsics = MTUnknown.intrinsics or {}

MTUnknown.intrinsics.passable = false
MTUnknown.intrinsics.opacity_red = 255
MTUnknown.intrinsics.opacity_green = 255
MTUnknown.intrinsics.opacity_blue = 255

MTUnknown.intrinsics.tile_character_code = 0x003f   -- question mark
MTUnknown.intrinsics.tile_bg_color_red = 0          -- black
MTUnknown.intrinsics.tile_bg_color_green = 0
MTUnknown.intrinsics.tile_bg_color_blue = 0
MTUnknown.intrinsics.tile_fg_color_red = 128        -- brown
MTUnknown.intrinsics.tile_fg_color_green = 64
MTUnknown.intrinsics.tile_fg_color_blue = 0

function MTUnknown.get_brief_description()
	return "Unexplored space."
end