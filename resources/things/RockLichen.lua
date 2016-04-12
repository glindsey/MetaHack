-- Definition of special functions for the RockLichen object type.

require "resources/things/Blob"

RockLichen = inheritsFrom(Blob, "RockLichen")
RockLichen.intrinsics.name = "rock lichen"
RockLichen.intrinsics.plural = "rock lichens"
RockLichen.intrinsics.creatable = true

RockLichen.intrinsics.maxhp = range(1, 1)

function RockLichen.get_description()
	return "A lichen that normally grows on a rock."
end
