#ifndef BODYPART_H_INCLUDED
#define BODYPART_H_INCLUDED

#include <string>

/// Enum class representing body parts or their analogous equivalents.
enum class BodyPart
{
  Body,
  Skin,
  Head,
  Ear,
  Eye,
  Nose,
  Mouth,
  Neck,
  Chest,
  Arm,
  Hand,
  Leg,
  Foot,
  Wing,
  Count
};

/// Struct identifying a place that an item is worn.
struct WearLocation
{
  WearLocation()
  {
    part = BodyPart::Count;
    number = 0;
  }

  BodyPart part;
  unsigned int number;
};

#endif // BODYPART_H_INCLUDED
