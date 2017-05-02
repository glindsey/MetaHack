#pragma once

#include <vector>

#include "boost/dynamic_bitset.hpp"

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "types/BodyPart.h"

// Struct that pairs a bodypart with the number of the bodypart that it is.
struct BodyPartPair
{
  BodyPart part;
  size_t number;

  friend bool operator<(BodyPartPair const& first, BodyPartPair const& second)
  {
    return (first.part != second.part) ? 
      (static_cast<unsigned int>(first.part) < static_cast<unsigned int>(second.part)) :
      (first.number < second.number);
  }
};

/// Component that indicates that an entity has multiple bodyparts.
/// Also deals with assigning wielded/worn items to bodyparts.
class ComponentBodyparts
{
public:

  friend void from_json(json const& j, ComponentBodyparts& obj);
  friend void to_json(json& j, ComponentBodyparts const& obj);

  json bodyPartToJSON(BodyPart part) const;

  void populateUsingJSON(BodyPart part, json const & j);

  /// Get typical count of a particular bodypart.
  unsigned int typicalCount(BodyPart part) const;

  /// Get actual count of a particular bodypart.
  size_t actualCount(BodyPart part) const;

  /// Select whether a particular bodypart exists.
  boost::dynamic_bitset<size_t>::reference exists(BodyPart part, unsigned int which);
  bool exists(BodyPart part, unsigned int which) const;

  /// Return whether the entity has any prehensile (grasping) bodyparts.
  bool hasPrehensileBodyPart() const;

  /// Return whether the specified entity is being worn, and if so, where.
  BodyPartPair getWornLocation(EntityId id) const;

  /// Return whether the specified entity is being wielded, and if so, where.
  BodyPartPair getWieldedLocation(EntityId id) const;

  /// Return what is being worn on the specified bodypart, if anything.
  EntityId getWornEntity(BodyPartPair part) const;

  /// Return what is being wielded by the specified bodypart, if anything.
  EntityId getWieldedEntity(BodyPartPair part) const;


protected:
  void setUpBodyparts(BodyPart part, unsigned int count);

private:
  /// Array of typical bodypart counts.
  std::array<unsigned int, static_cast<size_t>(BodyPart::MemberCount)> m_bodypartCount;

  /// Array of bitsets for bodypart counts.
  std::array<boost::dynamic_bitset<size_t>, static_cast<size_t>(BodyPart::MemberCount)> m_bodypartExistence;

  /// Map of items worn on bodyparts.
  std::map<BodyPartPair, EntityId> m_wornItems;

  /// Map of items wielded by bodyparts.
  std::map<BodyPartPair, EntityId> m_wieldedItems;

};
