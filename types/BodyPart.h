#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include "json.hpp"
using json = ::nlohmann::json;

/// Enum class representing body parts or their analogous equivalents.
enum class BodyPart
{
  Nowhere = 0,
  Unknown,
  Skin,
  Head,
  Face,
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
  Last,
  First = Nowhere
};

/// Bidirectional map of body parts to string equivalents.
using BodyPartBimap = boost::bimap<
  boost::bimaps::unordered_set_of<std::string>,
  boost::bimaps::unordered_set_of<BodyPart>
>;

namespace BodyParts
{
  BodyPartBimap::left_map const& strsToParts();
  BodyPartBimap::right_map const& partsToStrs();
} // end namespace

void from_json(json const& j, BodyPart& obj);
void to_json(json& j, BodyPart const& obj);
std::ostream& operator<<(std::ostream& os, BodyPart const& part);

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
    return (number * static_cast<size_t>(BodyPart::Last)) + static_cast<size_t>(part);
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
