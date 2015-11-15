-- Definition of special functions for the Human object type.

require "resources/things/Biped"

Human = inheritsFrom(Biped, "Human")
Human.intrinsics.name = "human"
Human.intrinsics.plural = "humans"

Human.defaults.proper_name = "John Doe"
Human.defaults.maxhp = range(6, 10)

Human.defaults.attribute_strength = range(13, 17)
Human.defaults.attribute_endurance = range(13, 17)
Human.defaults.attribute_vitality = range(13, 17)
Human.defaults.attribute_agility = range(13, 17)
Human.defaults.attribute_precision = range(13, 17)
Human.defaults.attribute_intelligence = range(13, 17)
Human.defaults.attribute_aura = range(13, 17)
Human.defaults.attribute_charisma = range(13, 17)
Human.defaults.attribute_vigilance = range(13, 17)
Human.defaults.attribute_luck = range(13, 17)

function Human.get_description()
	return "A human being."
end