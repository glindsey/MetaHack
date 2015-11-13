-- Definition of special functions for the Animal object type.

require "resources/things/Entity"

Animal = inheritsFrom(Entity, "Animal")
Animal.name = "[Animal]"

Animal.intrinsics.can_drink = true
Animal.intrinsics.can_eat = true
Animal.intrinsics.can_hear = true
Animal.intrinsics.can_move = true
Animal.intrinsics.can_see = true
Animal.intrinsics.can_smell = true

function Animal.get_description()
	return "A living organism that feeds on organic matter, typically having specialized sense organs and nervous system and able to respond rapidly to stimuli."
end