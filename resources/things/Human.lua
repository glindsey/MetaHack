-- Definition of special functions for the Human object type.

require "resources/things/Biped"

Human = inheritsFrom(Biped, "Human")
Human.intrinsics.name = "human"
Human.intrinsics.plural = "humans"

Human.intrinsics.attributes_strength = range(13, 17)
Human.intrinsics.attributes_endurance = range(13, 17)
Human.intrinsics.attributes_vitality = range(13, 17)
Human.intrinsics.attributes_agility = range(13, 17)
Human.intrinsics.attributes_precision = range(13, 17)
Human.intrinsics.attributes_intelligence = range(13, 17)
Human.intrinsics.attributes_aura = range(13, 17)
Human.intrinsics.attributes_suavity = range(13, 17)
Human.intrinsics.attributes_allure = range(13, 17)
Human.intrinsics.attributes_vigilance = range(13, 17)
Human.intrinsics.attributes_luck = range(13, 17)

Human.defaults.proper_name = "John Doe"
Human.defaults.hp = 10	-- @todo This is temporary
Human.defaults.maxhp = range(6, 10)

function Human.get_description()
	return "A human being."
end