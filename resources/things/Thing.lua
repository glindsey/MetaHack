-- Definition of special functions for the Thing object type.

Thing = Thing or {}

function Thing.on_create(id)
	messageLog_add("Thing.on_create() called, new ID = " .. id)
	return ActionResult.Success
end