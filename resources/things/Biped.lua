-- Definition of special functions for the Biped object type.

require "resources/things/Sapient"

Biped = inheritsFrom(Sapient, "Biped")
Biped.name = "[Biped]"

Biped.intrinsics.bodypart.body.count = 1;
Biped.intrinsics.bodypart.skin.count = 1;
Biped.intrinsics.bodypart.head.count = 1;
Biped.intrinsics.bodypart.ear.count = 2;
Biped.intrinsics.bodypart.eye.count = 2;
Biped.intrinsics.bodypart.nose.count = 1;
Biped.intrinsics.bodypart.mouth.count = 1;
Biped.intrinsics.bodypart.neck.count = 1;
Biped.intrinsics.bodypart.chest.count = 1;
Biped.intrinsics.bodypart.arm.count = 2;
Biped.intrinsics.bodypart.hand.count = 2;
Biped.intrinsics.bodypart.leg.count = 2;
Biped.intrinsics.bodypart.foot.count = 2;

function Biped.get_description()
	return "An animal that uses two legs for walking."
end
