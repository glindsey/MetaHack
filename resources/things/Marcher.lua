-- Definition of special functions for the Marcher object type.

require "resources/things/Biped"

Marcher = inheritsFrom(Biped, "Marcher")
Marcher.intrinsics.name = "marcher robot"
Marcher.intrinsics.plural = "marcher robot"
Marcher.intrinsics.creatable = true

Marcher.intrinsics.maxhp = range(1, 1)

function Marcher.get_brief_description()
    return "A testing Entity that endlessly marches left and right."
end

function Marcher.process()
    print "DEBUG: Marcher.process() called."
    return ActionResult.Success
end