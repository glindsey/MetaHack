#pragma once

#pragma once

#include "stdafx.h"

#include "types/common.h"

// Forward declaration
class Map;
class MapFactory;

/// Definition of an Map ID.
/// @see EntityId
class MapId
{
  friend struct std::hash<MapId>;

public:
  MapId()
    :
    m_id{ 0 }
  {}

  MapId(uint32_t id)
    :
    m_id{ id }
  {}

  MapId(std::string id)
    :
    m_id{ std::stoul(id) }
  {}

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(m_id);
  }

  friend void to_json(json& j, MapId const& id);
  friend void from_json(json const& j, MapId& id);

  /// Static method to return the MapId::Null() (nothingness) ID.
  static MapId Null()
  {
    return MapId();
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
  operator lua_Integer() const
  {
    return static_cast<lua_Integer>(m_id);
  }

  explicit operator uint64_t() const
  {
    return m_id;
  }

  operator std::string() const
  {
    return std::to_string(m_id);
  }

  bool operator<(MapId const& other) const
  {
    return (this->m_id < other.m_id);
  }

  bool operator<=(MapId const& other) const
  {
    return (this->m_id <= other.m_id);
  }

  bool operator>(MapId const& other) const
  {
    return !operator<=(other);
  }

  bool operator>=(MapId const& other) const
  {
    return !operator<(other);
  }

  bool operator==(MapId const& other) const
  {
    return (this->m_id == other.m_id);
  }

  bool operator==(uint32_t const& other) const
  {
    return (this->m_id == other);
  }

  bool operator!=(MapId const& other) const
  {
    return !operator==(other);
  }

  bool operator!=(uint32_t const& other) const
  {
    return !operator==(other);
  }

  MapId& operator++()
  {
    ++m_id;
    return *this;
  }

  MapId operator++(int)
  {
    MapId tmp(*this);
    operator++();
    return tmp;
  }

  friend std::ostream& operator<< (std::ostream& stream, MapId const& map)
  {
    stream << map.m_id;

    return stream;
  }

  /// Call to get the map associated with this.
  Map* operator->(void);

  /// Call to get the map associated with this.
  Map const* operator->(void) const;

private:
  uint32_t m_id;
};

namespace std
{
  /// Hash functor for MapId
  template<>
  struct hash<MapId>
  {
    std::size_t operator()(const MapId& key) const
    {
      std::size_t seed = 0;

      boost::hash_combine(seed, boost::hash_value(key.m_id));

      return seed;
    }
  };
}