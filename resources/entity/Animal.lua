-- Definition of special functions for the Animal object type.

require "resources/entity/DynamicEntity"

Animal = inheritsFrom(DynamicEntity, "Animal")
Animal.intrinsics.name = "[Animal]"
Animal.intrinsics.plural = "[Animal]"

Animal.intrinsics.can_attack = true
Animal.intrinsics.can_drink = true
Animal.intrinsics.can_eat = true
Animal.intrinsics.can_hear = true
Animal.intrinsics.can_move = true
Animal.intrinsics.can_see = true
Animal.intrinsics.can_smell = true
Animal.intrinsics.can_turn = true

function Animal.get_brief_description()
	return "A living organism that feeds on organic matter, typically having specialized sense organs and nervous system and able to respond rapidly to stimuli."
end