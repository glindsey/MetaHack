-- Definition of special functions for the Thing object type.

Thing = inheritsFrom(nil, "Thing")
Thing.intrinsics = {}
Thing.defaults = {}

-- Test code.
Thing.intrinsics.test1 = {}
Thing.intrinsics.test1.test1a = false
Thing.intrinsics.test1.test1b = true
Thing.intrinsics.test1.test1c = {}
Thing.intrinsics.test1.test1c.test1c1 = true
Thing.intrinsics.test2 = false
-- END test code

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
			error("Couldn't retrieve " .. name .. ": " .. firstPart .. " is not a table")
		end
		
		-- Traverse down the nested tables.
		tableToSearch = tableToSearch[firstPart];
		
		-- Find the next dot.
		dotLocation = string.find(secondPart, ".", 1, true)
	end
	
	-- When we get here, secondPart should be the final result we want to return.
	
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
			error("Couldn't retrieve " .. name .. ": " .. firstPart .. " is not a table")
		end
		
		-- Traverse down the nested tables.
		tableToSearch = tableToSearch[firstPart];
		
		-- Find the next dot.
		dotLocation = string.find(secondPart, ".", 1, true)
	end
	
	-- When we get here, secondPart should be the final result we want to set.
	
	tableToSearch[secondPart] = value
end