-- Definition of special functions for the CoinGold object type.

require "resources/things/Coin"

CoinGold = inheritsFrom(Coin, "CoinGold")
CoinGold.intrinsics.name = "gold coin"
CoinGold.intrinsics.plural = "gold coins"
CoinGold.intrinsics.creatable = true

function CoinGold.get_brief_description()
	return "A coin made out of gold."
end