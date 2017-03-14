-- Definition of the Unknown MapTile type.

MTUnknown = inheritsFrom(nil, "MTUnknown")
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

function MTUnknown:get_intrinsic(name)
	--print("DEBUG: Looking for " .. name .. " in " .. self.type)
	local result = self.intrinsics[name]
	
	if (result == nil) then
		--print("DEBUG: Not found, moving up the parent chain")
		local superclass = self.superClass()
		
		if (superclass ~= nil) then
			return self.superClass():get_intrinsic(name)
		else
			error("Traversed all the way to root class but could not find " .. name)
		end
	else
		--print("Found result = " .. tostring(result))
		return result
	end		
end

function MTUnknown:set_intrinsic(name, value)
	self.intrinsics[name] = value
end
