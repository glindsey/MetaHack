#pragma once

#include "stdafx.h"

/// Enum class representing body parts or their analogous equivalents.
enum class BodyPart
{
  Nowhere = 0,
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
  Finger,
  Torso,
  Leg,
  Foot,
  Toe,
  Wing,
  Tail,
  PTail,
  MemberCount
};

inline std::ostream& operator<<(std::ostream& os, BodyPart const& part)
{
  switch (part)
  {
    case BodyPart::Nowhere: os << "Nowhere"; break;
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
    case BodyPart::Finger: os << "Finger"; break;
    case BodyPart::Torso: os << "Torso"; break;
    case BodyPart::Leg: os << "Leg"; break;
    case BodyPart::Foot: os << "Foot"; break;
    case BodyPart::Toe: os << "Toe"; break;
    case BodyPart::Wing: os << "Wing"; break;
    case BodyPart::Tail: os << "Tail"; break;
    case BodyPart::PTail: os << "PTail"; break;
    case BodyPart::MemberCount: os << "MemberCount"; break;
    default: os << "???"; break;
  }

  return os;
}

/// Struct identifying a place that an item is worn.
struct BodyLocation
{
  BodyLocation()
    :
    part{ BodyPart::Nowhere }, number{ 0 }
  {}

  BodyLocation(BodyPart part_, unsigned int number_)
    :
    part{ part_ }, number{ number_ }
  {}

  BodyPart part;
  size_t number;

  inline size_t index() const
  {
    return (number * static_cast<size_t>(BodyPart::MemberCount)) +
      static_cast<size_t>(part);
  }

};

namespace std
{
  /// Hash functor for BodyLocation
  template<>
  struct hash<BodyLocation>
  {
    std::size_t operator()(const BodyLocation& key) const
    {
      std::size_t seed = 0;

      boost::hash_combine(seed, boost::hash_value(key.number));
      boost::hash_combine(seed, boost::hash_value(key.part));

      return seed;
    }
  };
}

inline bool operator==(const BodyLocation& lhs, const BodyLocation& rhs)
{
  return lhs.index() == rhs.index();
}

inline bool operator!=(const BodyLocation& lhs, const BodyLocation& rhs)
{
  return !operator==(lhs, rhs);
}

inline bool operator<(const BodyLocation& lhs, const BodyLocation& rhs)
{
  return lhs.index() < rhs.index();
}

inline bool operator>(const BodyLocation& lhs, const BodyLocation& rhs)
{
  return operator<(rhs, lhs);
}

inline bool operator<=(const BodyLocation& lhs, const BodyLocation& rhs)
{
  return !operator>(lhs, rhs);
}

inline bool operator>=(const BodyLocation& lhs, const BodyLocation& rhs)
{
  return !operator<(lhs, rhs);
}
