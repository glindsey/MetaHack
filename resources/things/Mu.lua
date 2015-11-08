-- Definition of special functions for the Mu (nothingness) object type.

require "resources/things/Thing"

Mu = inheritsFrom(Thing)

function Mu.on_create(id)
	messageLog_add("Mu.on_create() called, new ID = " .. id)
	return ActionResult.Success
end