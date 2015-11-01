-- Definition of special functions for the Human object type.

Human = Human or {}

function Human.on_create(id)
	messageLog_add("Human.on_create() called, new ID = " .. id)
	return ActionResult.Success
end

function Human.process(id)
	local x, y = thing_get_coords(id);
	messageLog.add("Human " .. id .. " is at (" .. x .. ", " .. y .. ")")
	return ActionResult.Success
end