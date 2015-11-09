-- Definition of special functions for the CoinGold object type.

require "resources/things/Coin"

CoinGold = inheritsFrom(Coin, "CoinGold")
CoinGold.name = "gold coin"

function CoinGold.get_description()
	return "A coin made out of gold."
end