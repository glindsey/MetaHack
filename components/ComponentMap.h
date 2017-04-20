#pragma once

#include "entity/EntityId.h"

#include <boost/optional.hpp>

#include "json.hpp"
using json = ::nlohmann::json;

/// Represents a collection of a particular component mapped to the entities that contain it.
template <typename T>
class ComponentMap final
{
public:
  ComponentMap() {}
  ~ComponentMap() {}

  /// Add an entity to this component map, with the default component value.
  void addDefault(EntityId id)
  {
    m_componentMap[id] = T();
  }

  /// Clone a value from one key to another, if the first key exists.
  void cloneIfExists(EntityId first, EntityId second)
  {
    if (existsFor(first))
    {
      m_componentMap[second] = m_componentMap[first];
    }
  }

  bool existsFor(EntityId id) const
  {
    return m_componentMap.count(id) != 0ULL;
  }

  T& of(EntityId id)
  {
    return m_componentMap.at(id);
  }

  T const& of(EntityId id) const
  {
    return m_componentMap.at(id);
  }

  /// Remove an entity from this component map.
  /// If the entity isn't already present, just returns without doing anything.
  void remove(EntityId id)
  {
    m_componentMap.erase(id);
  }

  T& operator[](EntityId id)
  {
    if (!existsFor(id))
    {
      m_componentMap[id] = T();
    }

    return m_componentMap[id];
  }

  T const& valueOrDefault(EntityId id) const
  {
    static T defaultValue;

    if (!existsFor(id))
    {
      return defaultValue;
    }

    return m_componentMap.at(id);
  }

  T const& valueOr(EntityId id, T const& defaultValue) const
  {
    if (!existsFor(id))
    {
      return defaultValue;
    }

    return m_componentMap[id];
  }

  /// Get the map itself for iterating through.
  std::unordered_map<EntityId, T>& data()
  {
    return m_componentMap;
  }

  std::unordered_map<EntityId, T> const& data() const
  {
    return m_componentMap;
  }

  friend void from_json(json const& j, ComponentMap& obj)
  {
    obj.m_componentMap.clear();

    if (j.is_object() && j.size() != 0)
    {
      for (auto citer = j.cbegin(); citer != j.cend(); ++citer)
      {
        obj.m_componentMap[citer.key()] = citer.value();
      }
    }
  }

  friend void to_json(json& j, ComponentMap const& obj)
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

  /// @todo Implement component modifiers.
};