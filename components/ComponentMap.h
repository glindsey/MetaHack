#pragma once

#include "entity/EntityId.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// Represents a collection of a particular component mapped to the entities that contain it.
template <typename T>
class ComponentMap final
{
public:
  ComponentMap() {}
  ~ComponentMap() {}

  T& at(EntityId id)
  {
    return m_componentMap.at(id);
  }

  T const& at(EntityId id) const
  {
    return m_componentMap.at(id);
  }

  unsigned int count(EntityId id)
  {
    return m_componentMap.count(id);
  }

  bool exists(EntityId id)
  {
    return count(id) != 0;
  }

  T& operator[](EntityId id)
  {
    if (!exists(id))
    {
      m_componentMap[id] = T();
    }

    return m_componentMap[id];
  }

  friend void from_json(json& j, ComponentMap const& obj)
  {
    obj.m_componentMap.clear();
    for (auto citer = j.cbegin(); citer != j.cend(); ++citer)
    {
      obj.m_componentMap[citer.key()] = citer.value();
    }
  }

  friend void to_json(json const& j, ComponentMap& obj)
  {
    j = json::object();
    for (auto& pair : obj.m_componentMap)
    {
      j[pair.first] = pair.second;
    }
  }

protected:

private:
  std::unordered_map<EntityId, T> m_componentMap;
};