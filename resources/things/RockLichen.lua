-- Definition of special functions for the RockLichen object type.

require "resources/things/Blob"

RockLichen = inheritsFrom(Blob, "RockLichen")
RockLichen.name = "rock lichen"

function RockLichen.get_description()
	return "A lichen that normally grows on a rock."
end