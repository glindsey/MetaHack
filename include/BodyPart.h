#ifndef BODYPART_H_INCLUDED
#define BODYPART_H_INCLUDED

#include <string>

/// Enum class representing body parts or their analogous equivalents.
enum class BodyPart
{
  Nowhere,
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

  inline unsigned int index() const
  {
    return (number * static_cast<unsigned int>(BodyPart::Count)) +
            static_cast<unsigned int>(part);
  }
};

inline bool operator==(const WearLocation& lhs, const WearLocation& rhs)
{
  return lhs.index() == rhs.index();
}

inline bool operator!=(const WearLocation& lhs, const WearLocation& rhs)
{
  return !operator==(lhs, rhs);
}

inline bool operator<(const WearLocation& lhs, const WearLocation& rhs)
{
  return lhs.index() < rhs.index();
}

inline bool operator>(const WearLocation& lhs, const WearLocation& rhs)
{
  return operator<(rhs, lhs);
}

inline bool operator<=(const WearLocation& lhs, const WearLocation& rhs)
{
  return !operator>(lhs, rhs);
}

inline bool operator>=(const WearLocation& lhs, const WearLocation& rhs)
{
  return !operator<(lhs, rhs);
}

#endif // BODYPART_H_INCLUDED
