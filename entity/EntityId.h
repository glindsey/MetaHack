#pragma once

#include <boost/functional/hash.hpp>

#include "types/common.h"

// Lua includes
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "json.hpp"
using json = ::nlohmann::json;

// Forward declaration
class Entity;
class EntityFactory;

/// Definition of an Entity ID.
/// While this COULD just be an integer, the json class requires map keys to
/// be strings. I'm hoping this doesn't slow things down too much during
/// serialization/deserialization.
///
/// Although internally this is a 64-bit integer, LuaJIT stores all numbers
/// as doubles, which means we are limited to 2^53 valid IDs. That's still
/// an enormous number, though (9 quadrillion IDs), so I THINK we'll be okay.
class EntityId
{
  friend struct std::hash<EntityId>;

public:
  EntityId();
  EntityId(uint64_t id);
  EntityId(std::string id);

  friend void to_json(json& j, EntityId const& id);
  friend void from_json(json const& j, EntityId& id);

  /// Static method to return the EntityId::Mu() (nothingness) ID.
  static EntityId Mu();

  explicit operator bool() const;
  bool operator!() const;
  operator lua_Integer() const;
  explicit operator uint64_t() const;
  operator std::string() const;
  bool operator<(EntityId const& other) const;
  bool operator<=(EntityId const& other) const;
  bool operator>(EntityId const& other) const;
  bool operator>=(EntityId const& other) const;
  bool operator==(EntityId const& other) const;
  bool operator==(uint64_t const& other) const;
  bool operator!=(EntityId const& other) const;
  bool operator!=(uint64_t const& other) const;
  EntityId& operator++();
  EntityId operator++(int);
  EntityId& operator--();
  EntityId operator--(int);

  friend std::ostream& operator<<(std::ostream& stream, EntityId const& entity);

  /// Call to get the entity associated with this.
  Entity* operator->(void);

  /// Call to get the entity associated with this.
  Entity const* operator->(void) const;

private:
  uint64_t m_id;
};

namespace std
{
  /// Hash functor for EntityId
  template<>
  struct hash<EntityId>
  {
    std::size_t operator()(const EntityId& key) const
    {
      std::size_t seed = 0;

      boost::hash_combine(seed, boost::hash_value(key.m_id));

      return seed;
    }
  };
}
