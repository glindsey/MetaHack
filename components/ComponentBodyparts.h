#pragma once

#include <vector>

#include "boost/dynamic_bitset.hpp"

#include "json.hpp"
using json = ::nlohmann::json;

#include "types/BodyPart.h"

/// Component that indicates that an entity has multiple bodyparts.
class ComponentBodyparts
{
public:

  friend void from_json(json const& j, ComponentBodyparts& obj);
  friend void to_json(json& j, ComponentBodyparts const& obj);

  /// Get typical count of a particular bodypart.
  unsigned int typicalCount(BodyPart part);

  /// Get actual count of a particular bodypart.
  size_t actualCount(BodyPart part);

  // Select whether a particular bodypart exists.
  boost::dynamic_bitset<size_t>::reference exists(BodyPart part, unsigned int which);
  bool exists(BodyPart part, unsigned int which) const;

protected:
  void setUpBodyparts(BodyPart part, unsigned int count);

private:
  /// Array of typical bodypart counts.
  std::array<unsigned int, static_cast<size_t>(BodyPart::MemberCount)> m_bodypartCount;

  /// Array of bitsets for bodypart counts.
  std::array<boost::dynamic_bitset<size_t>, static_cast<size_t>(BodyPart::MemberCount)> m_bodypartExistence;
};