-- Definition of the Unknown MapTile type.

MTUnknown = inheritsFrom(nil, "MTUnknown")
MTUnknown.intrinsics.name = "unknown"

MTUnknown.intrinsics = MTUnknown.intrinsics or {}
MTUnknown.defaults = MTUnknown.defaults or {}

MTUnknown.intrinsics.passable = false
MTUnknown.intrinsics.opaque = true
MTUnknown.intrinsics.opacity_red = 255;
MTUnknown.intrinsics.opacity_green = 255;
MTUnknown.intrinsics.opacity_blue = 255;


function MTUnknown.get_description()
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

function MTUnknown:get_default(name)
	--print("DEBUG: Looking for " .. name .. " in " .. self.type)
	local result = self.defaults[name]
	
	if (result == nil) then
		--print("DEBUG: Not found, moving up the parent chain")
		local superclass = self.superClass()
		
		if (superclass ~= nil) then		
			return self.superClass():get_default(name)
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
