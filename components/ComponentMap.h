#pragma once

#include "AssertHelper.h"
#include "entity/EntityId.h"

#include <boost/optional.hpp>

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

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
      std::string className = typeid(T).name();
      CLOG(TRACE, "Component") << "Creating new " << typeid(T).name() << " for ID " << id;
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
      Assert("Component", existsFor(id), "Non-existent component of entity " << id << " requested");
      return m_componentMap.at(id);
    }

    T const& of(EntityId id) const
    {
      Assert("Component", existsFor(id), "Non-existent component of entity " << id << " requested");
      return m_componentMap.at(id);
    }

    /// Update the component for a specific ID depending on the provided JSON.
    ///   * If the JSON is empty (null):
    ///     * If the component already exists, do nothing.
    ///     * If the component doesn't exist, add it using default values.
    ///   * If the JSON is the string "DELETE" (case sensitive), remove the existing component data for that ID.
    ///   * Otherwise, replace the component data with the provided JSON.
    void update(EntityId id, json const& newData)
    {
      if (newData.is_null())
      {
        if (!existsFor(id))
        {
          std::string className = typeid(T).name();
          CLOG(TRACE, "Component") << "Creating new " << typeid(T).name() << " for ID " << id;
          m_componentMap.emplace(std::make_pair(id, T()));
        }
      }
      else
      {
        if (newData.is_string() && newData.get<std::string>() == "DELETE")
        {
          remove(id);
        }
        else
        {
          // (This looks a little funky but makes sure new component is generated
          //  if missing.)
          operator[](id) = newData;
        }
      }
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
        std::string className = typeid(T).name();
        CLOG(TRACE, "Component") << "Creating new " << typeid(T).name() << " for ID " << id;
        m_componentMap.emplace(std::make_pair(id, T()));
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

      return m_componentMap.at(id);
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

      if (j.is_object())
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

    /// @todo Implement component modifiers, possibly with some sort of
    ///       "snapshot" mechanism for specific components.
  };

} // end namespace
