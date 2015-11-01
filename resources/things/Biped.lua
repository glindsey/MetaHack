-- Definition of special functions for the Biped object type.

Biped = Biped or {}

function Biped.on_create(id)
	messageLog_add("Biped.on_create() called, new ID = " .. id)
	return ActionResult.Success
end

function Biped.process(id)
	local x, y = thing_get_coords(id);
	messageLog.add("Biped " .. id .. " is at (" .. x .. ", " .. y .. ")")
	return ActionResult.Success
end