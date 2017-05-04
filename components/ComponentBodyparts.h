#pragma once

#include <vector>

#include "boost/dynamic_bitset.hpp"

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "types/BodyPart.h"

/// Component that indicates that an entity has multiple bodyparts.
/// Also deals with assigning wielded/worn items to bodyparts.
class ComponentBodyparts
{
public:
  ComponentBodyparts();
  ComponentBodyparts(ComponentBodyparts const& other);
  ComponentBodyparts& operator=(ComponentBodyparts const& other);

  friend void from_json(json const& j, ComponentBodyparts& obj);
  friend void to_json(json& j, ComponentBodyparts const& obj);

  json bodyPartToJSON(BodyPart part) const;

  void populateUsingJSON(BodyPart part, json const & j);

  /// Get typical count of a particular bodypart.
  unsigned int typicalCount(BodyPart part) const;

  /// Get actual count of a particular bodypart.
  size_t actualCount(BodyPart part) const;

  /// Select whether a particular bodypart exists.
  boost::dynamic_bitset<size_t>::reference exists(BodyPart part, size_t which);
  bool exists(BodyPart part, size_t which) const;
  boost::dynamic_bitset<size_t>::reference exists(BodyLocation location);
  bool exists(BodyLocation location) const;

  /// Return whether the entity has any prehensile (grasping) bodyparts.
  bool hasPrehensileBodyPart() const;

  /// Return whether the specified entity is being worn, and if so, where.
  /// If it is not worn, returns { BodyPart::Nowhere, 0 }.
  BodyLocation getWornLocation(EntityId id) const;

  /// Return whether the specified entity is being wielded, and if so, where.
  /// If it is not worn, returns { BodyPart::Nowhere, 0 }.
  BodyLocation getWieldedLocation(EntityId id) const;

  /// Return what is being worn on the specified bodypart, if anything.
  /// If nothing is worn there, returns EntityId::Mu().
  EntityId getWornEntity(BodyLocation location) const;

  /// Return what is being wielded by the specified bodypart, if anything.
  /// If nothing is wielded there, returns EntityId::Mu().
  EntityId getWieldedEntity(BodyLocation location) const;

  /// Remove an entity from being worn on anything.
  void removeWornEntity(EntityId id);

  /// Remove an entity from being wielded anywhere.
  void removeWieldedEntity(EntityId id);

  /// Remove anything from being worn on a location.
  void removeWornLocation(BodyLocation location);

  /// Remove anything from being wielded by a bodypart.
  void removeWieldedLocation(BodyLocation location);

  /// Wear an entity on a bodypart.
  /// Removes the entity from being worn anywhere else first.
  /// Returns true if successful, false if the bodypart is not valid.
  bool wearEntity(EntityId id, BodyLocation location);

  /// Wield an entity with a bodypart.
  /// Removes the entity from being wielded anywhere else first.
  /// Returns true if successful, false if the bodypart is not valid.
  bool wieldEntity(EntityId id, BodyLocation location);

protected:
  void setUpBodyparts(BodyPart part, unsigned int count);

private:
  /// Array of typical bodypart counts.
  std::array<unsigned int, static_cast<size_t>(BodyPart::MemberCount)> m_bodypartCount;

  /// Array of bitsets for bodypart counts.
  std::array<boost::dynamic_bitset<size_t>, static_cast<size_t>(BodyPart::MemberCount)> m_bodypartExistence;

  /// Map of items worn on bodyparts.
  std::map<BodyLocation, EntityId> m_wornItems;

  /// Map of items wielded by bodyparts.
  std::map<BodyLocation, EntityId> m_wieldedItems;

};
