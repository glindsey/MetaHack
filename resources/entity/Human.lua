-- Definition of special functions for the Human object type.

require "resources/entity/Biped"

Human = inheritsFrom(Biped, "Human")
Human.intrinsics.name = "human"
Human.intrinsics.plural = "humans"
Human.intrinsics.creatable = true

Human.intrinsics.proper_name = "John Doe"
Human.intrinsics.maxhp = range(6, 10)

Human.intrinsics.attribute_strength = range(13, 17)
Human.intrinsics.attribute_endurance = range(13, 17)
Human.intrinsics.attribute_vitality = range(13, 17)
Human.intrinsics.attribute_agility = range(13, 17)
Human.intrinsics.attribute_precision = range(13, 17)
Human.intrinsics.attribute_intelligence = range(13, 17)
Human.intrinsics.attribute_aura = range(13, 17)
Human.intrinsics.attribute_charisma = range(13, 17)
Human.intrinsics.attribute_vigilance = range(13, 17)
Human.intrinsics.attribute_luck = range(13, 17)

function Human.get_brief_description()
	return "A human being.", PropertyType.String
end