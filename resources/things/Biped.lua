-- Definition of special functions for the Biped object type.

require "resources/things/Sapient"

Biped = inheritsFrom(Sapient, "Biped")
Biped.intrinsics.name = "[Biped]"
Biped.intrinsics.plural = "[Bipeds]"

Biped.intrinsics.bodypart_body_count = 1
Biped.intrinsics.bodypart_skin_count = 1
Biped.intrinsics.bodypart_head_count = 1
Biped.intrinsics.bodypart_ear_count = 2
Biped.intrinsics.bodypart_eye_count = 2
Biped.intrinsics.bodypart_nose_count = 1
Biped.intrinsics.bodypart_mouth_count = 1
Biped.intrinsics.bodypart_neck_count = 1
Biped.intrinsics.bodypart_chest_count = 1
Biped.intrinsics.bodypart_arm_count = 2
Biped.intrinsics.bodypart_hand_count = 2
Biped.intrinsics.bodypart_leg_count = 2
Biped.intrinsics.bodypart_foot_count = 2

function Biped.get_description()
	return "An animal that uses two legs for walking."
end
