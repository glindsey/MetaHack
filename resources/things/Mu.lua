-- Definition of special functions for the Mu (nothingness) object type.

Mu = Mu or {}

function Mu.on_create(id)
	messageLog_add("Mu.on_create() called, new ID = " .. id)
	return ActionResult.Success
end