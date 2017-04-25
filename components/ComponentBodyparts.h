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
  /// Vector of typical bodypart counts.
  std::vector<unsigned int> m_bodypartCount;

  /// Vector of bitsets for bodypart counts.
  std::vector<boost::dynamic_bitset<size_t>> m_bodypartExistence;
};
