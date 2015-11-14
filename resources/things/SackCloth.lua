-- Definition of special functions for the SackCloth object type.

require "resources/things/Container"

SackCloth = inheritsFrom(Container, "SackCloth")
SackCloth.intrinsics.name = "cloth sack"
SackCloth.intrinsics.plural = "cloth sacks"

SackCloth.intrinsics.inventory_size = 10
SackCloth.intrinsics.openable = false

SackCloth.defaults.open = true

function SackCloth.get_description()
	return "A sack made out of cloth."
end
