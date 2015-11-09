-- Definition of the Thing object type, which all others inherit from.

Thing = inheritsFrom(nil, "Thing")
Thing.name = "[Thing]"

Thing.intrinsics = {}
Thing.defaults = {}

Thing.intrinsics.bodypart = {}
Thing.intrinsics.bodypart.body = {}
Thing.intrinsics.bodypart.body.name = "body"
Thing.intrinsics.bodypart.body.plural = "bodies"
Thing.intrinsics.bodypart.skin = {}
Thing.intrinsics.bodypart.skin.name = "skin"
Thing.intrinsics.bodypart.head = {}
Thing.intrinsics.bodypart.head.name = "head"
Thing.intrinsics.bodypart.ear = {}
Thing.intrinsics.bodypart.ear.name = "ear"
Thing.intrinsics.bodypart.eye = {}
Thing.intrinsics.bodypart.eye.name = "eye"
Thing.intrinsics.bodypart.nose = {}
Thing.intrinsics.bodypart.nose.name = "nose"
Thing.intrinsics.bodypart.mouth = {}
Thing.intrinsics.bodypart.mouth.name = "mouth"
Thing.intrinsics.bodypart.neck = {}
Thing.intrinsics.bodypart.neck.name = "neck"
Thing.intrinsics.bodypart.chest = {}
Thing.intrinsics.bodypart.chest.name = "chest"
Thing.intrinsics.bodypart.arm = {}
Thing.intrinsics.bodypart.arm.name = "arm"
Thing.intrinsics.bodypart.hand = {}
Thing.intrinsics.bodypart.hand.name = "hand"
Thing.intrinsics.bodypart.leg = {}
Thing.intrinsics.bodypart.leg.name = "leg"
Thing.intrinsics.bodypart.foot = {}
Thing.intrinsics.bodypart.foot.name = "foot"
Thing.intrinsics.bodypart.foot.plural = "feet"
Thing.intrinsics.bodypart.wing = {}
Thing.intrinsics.bodypart.wing.name = "wing"
Thing.intrinsics.bodypart.tail = {}
Thing.intrinsics.bodypart.tail.name = "tail"
Thing.intrinsics.inventory_size = 0;
Thing.intrinsics.lockable = false;
Thing.intrinsics.movable = true;
Thing.intrinsics.opaque = true;
Thing.intrinsics.openable = false;
Thing.intrinsics.physical_mass = 0;
  
Thing.defaults.locked = false;
Thing.defaults.open = true;
Thing.defaults.quantity = 1;

function Thing.get_description(id)
	return "Thing #" .. id .. ", which has no description associated with it."
end

function Thing.get_tile_offset(id, frame)
	return 0, 0
end

function Thing.on_create(id)
	messageLog_add("Thing.on_create() called, new ID = " .. id)
	return ActionResult.Success
end

function Thing.on_lit_by(id)
	return ActionResult.Success
end

function Thing.process()
	return ActionResult.Success
end

function Thing:get(name)
	if type(name) ~= "string" then
		error("Name is not a string")
	end
	
	local tableToSearch = self
	local firstPart = ""
	local secondPart = name
	
	-- Look for a dot in the string.
	local dotLocation = string.find(secondPart, ".", 1, true)
	
	while (dotLocation ~= nil) do
		-- Strip off the part before the dot.
		firstPart = string.sub(secondPart, 1, dotLocation - 1)
		secondPart = string.sub(secondPart, dotLocation + 1)
		
		-- Make sure "tableToSearch" is a table.
		if type(tableToSearch) ~= "table" then
			return nil;
		end
		
		-- Traverse down the nested tables.
		tableToSearch = tableToSearch[firstPart];
		
		-- Find the next dot.
		dotLocation = string.find(secondPart, ".", 1, true)
	end
	
	-- When we get here, secondPart should be the final result we want to return.
	-- Or, if secondPart doesn't exist, it will be nil.
	
	return tableToSearch[secondPart]
	
end

function Thing:set(name, value)
	if type(name) ~= "string" then
		error("Name is not a string")
	end
	
	local tableToSearch = self
	local firstPart = ""
	local secondPart = name
	
	-- Look for a dot in the string.
	local dotLocation = string.find(secondPart, ".", 1, true)
	
	while (dotLocation ~= nil) do
		-- Strip off the part before the dot.
		firstPart = string.sub(secondPart, 1, dotLocation - 1)
		secondPart = string.sub(secondPart, dotLocation + 1)
		
		-- Make sure "tableToSearch" is a table.
		if type(tableToSearch) ~= "table" then
			error("Couldn't set " .. name .. ": " .. firstPart .. " is not a table")
		end
		
		-- Traverse down the nested tables, creating as we go.
		if tableToSearch[firstPart] == nil then
			tableToSearch[firstPart] = {}
		end
		
		tableToSearch = tableToSearch[firstPart];
		
		-- Find the next dot.
		dotLocation = string.find(secondPart, ".", 1, true)
	end
	
	-- When we get here, secondPart should be the final result we want to set.
	if type(tableToSearch[secondPart]) == "table" then
		error("Couldn't set " .. name .. ": " .. secondPart .. " already exists and is a table")
	end
	
	tableToSearch[secondPart] = value
	
end