-- Definition of special functions for the Human object type.

require "resources/things/Biped"

Human = inheritsFrom(Biped, "Human")
Human.name = "human"

Human.intrinsics.attributes.strength = range(13, 17)
Human.intrinsics.attributes.endurance = range(13, 17)
Human.intrinsics.attributes.vitality = range(13, 17)
Human.intrinsics.attributes.agility = range(13, 17)
Human.intrinsics.attributes.intelligence = range(13, 17)
Human.intrinsics.attributes.magicdefense = range(13, 17)
Human.intrinsics.attributes.charisma = range(13, 17)
Human.intrinsics.attributes.allure = range(13, 17)
Human.intrinsics.attributes.attentiveness = range(13, 17)
Human.intrinsics.attributes.luck = range(13, 17)

Human.defaults.proper_name = "John Doe"
Human.defaults.hp = 10	-- @todo This is temporary
Human.defaults.maxhp = range(6, 10)

function Human.get_description()
	return "A human being."
end