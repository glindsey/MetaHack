#ifndef THINGREF_H
#define THINGREF_H

#include "stdafx.h"

#include "json.hpp"
using json = ::nlohmann::json;

#include "types/common.h"

// Forward declaration
class Entity;
class EntityPool;

/// Definition of a ID/version pair.
/// Although internally this is a 64-bit integer, LuaJIT stores all numbers
/// as doubles, which means we are limited to 2^53 valid IDs. That's still
/// an enormous number, though (9 quadrillion IDs), so I THINK we'll be okay.
class EntityId
{
  friend struct std::hash<EntityId>;

public:
  EntityId()
    :
    m_id{ 0 }
  {}

  EntityId(uint64_t id)
    :
    m_id{ id }
  {}

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(m_id);
  }

  friend void to_json(json& j, EntityId const& id);
  friend void from_json(json const& j, EntityId& id);

  /// Static method to return the EntityId::Mu() (nothingness) ID.
  static EntityId Mu()
  {
    return EntityId();
  }

  explicit operator bool() const
  {
    return m_id != 0;
  }

  bool operator!() const
  {
    return !operator bool();
  }

  /// Convert to Lua integer.
  /// @warning If for some crazy reason m_id is somehow larger than
  ///          2^53, this will NOT WORK PROPERLY due to precision loss.
  ///          This is because LuaJIT stores all numbers as doubles.
  operator lua_Integer() const
  {
    return static_cast<lua_Integer>(m_id);
  }

  operator uint64_t() const
  {
    return m_id;
  }

  operator std::string() const
  {
    return str(m_id);
  }

  friend std::ostream& operator<<(std::ostream& os, EntityId& id)
  {
    os << str(id);
    return os;
  }

  bool operator<(EntityId const& other) const
  {
    return (this->m_id < other.m_id);
  }

  bool operator<=(EntityId const& other) const
  {
    return (this->m_id <= other.m_id);
  }

  bool operator>(EntityId const& other) const
  {
    return !operator<=(other);
  }

  bool operator>=(EntityId const& other) const
  {
    return !operator<(other);
  }

  bool operator==(EntityId const& other) const
  {
    return (this->m_id == other.m_id);
  }

  bool operator==(uint64_t const& other) const
  {
    return (this->m_id == other);
  }

  bool operator!=(EntityId const& other) const
  {
    return !operator==(other);
  }

  bool operator!=(uint64_t const& other) const
  {
    return !operator==(other);
  }

  friend std::ostream& operator<< (std::ostream& stream, EntityId const& entity)
  {
    stream << entity.m_id;

    return stream;
  }

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

#endif // THINGREF_H
