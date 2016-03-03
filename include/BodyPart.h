#ifndef BODYPART_H_INCLUDED
#define BODYPART_H_INCLUDED

#include "stdafx.h"

/// Enum class representing body parts or their analogous equivalents.
enum class BodyPart
{
  Nowhere = 0,
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
  Tail,
  Count
};

inline std::ostream& operator<<(std::ostream& os, BodyPart const& part)
{
  switch (part)
  {
    case BodyPart::Nowhere: os << "Nowhere"; break;
    case BodyPart::Body: os << "Body"; break;
    case BodyPart::Skin: os << "Skin"; break;
    case BodyPart::Head: os << "Head"; break;
    case BodyPart::Ear: os << "Ear"; break;
    case BodyPart::Eye: os << "Eye"; break;
    case BodyPart::Nose: os << "Nose"; break;
    case BodyPart::Mouth: os << "Mouth"; break;
    case BodyPart::Neck: os << "Neck"; break;
    case BodyPart::Chest: os << "Chest"; break;
    case BodyPart::Arm: os << "Arm"; break;
    case BodyPart::Hand: os << "Hand"; break;
    case BodyPart::Leg: os << "Leg"; break;
    case BodyPart::Foot: os << "Foot"; break;
    case BodyPart::Wing: os << "Wing"; break;
    case BodyPart::Tail: os << "Tail"; break;
    case BodyPart::Count: os << "Count"; break;
    default: os << "???"; break;
  }

  return os;
}

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

namespace std
{
  /// Hash functor for WearLocation
  template<>
  struct hash<WearLocation>
  {
    std::size_t operator()(const WearLocation& key) const
    {
      std::size_t seed = 0;

      boost::hash_combine(seed, boost::hash_value(key.number));
      boost::hash_combine(seed, boost::hash_value(key.part));

      return seed;
    }
  };
}

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
