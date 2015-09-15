-- Definition of special functions for the Rock object type.

Rock = Rock or {}

function Rock.on_create(id)
	messageLog_add("Rock.on_create() called, new ID = " .. id)
	return 0
end