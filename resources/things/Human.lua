-- Definition of special functions for the Human object type.

Human = Human or {}

function Human.process(id)
	local x, y = thing_get_coords(id);
	messageLog_add("Human " .. id .. " is at (" .. x .. ", " .. y .. ")")
	return ActionResult.Success
end