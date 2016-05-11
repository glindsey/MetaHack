#ifndef THINGREF_H
#define THINGREF_H

#include "stdafx.h"

// Forward declaration
class Thing;
class ThingManager;

/// Definition of a ID/version pair.
/// Although internally this is a 64-bit integer, LuaJIT stores all numbers
/// as doubles, which means we are limited to 2^53 valid IDs. That's still
/// an enormous number, though (9 quadrillion IDs), so I THINK we'll be okay.
class ThingId
{
  friend struct std::hash<ThingId>;

public:
  ThingId::ThingId()
    :
    m_id{ 0 }
  {}

  ThingId::ThingId(uint64_t id)
    :
    m_id{ id }
  {}

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(m_id);
  }

  /// Static method to return the ThingId::Mu() (nothingness) ID.
  static ThingId Mu()
  {
    return ThingId();
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

  bool ThingId::operator<(ThingId const& other) const
  {
    return (this->m_id < other.m_id);
  }

  bool ThingId::operator<=(ThingId const& other) const
  {
    return (this->m_id <= other.m_id);
  }

  bool ThingId::operator>(ThingId const& other) const
  {
    return !operator<=(other);
  }

  bool ThingId::operator>=(ThingId const& other) const
  {
    return !operator<(other);
  }

  bool ThingId::operator==(ThingId const& other) const
  {
    return (this->m_id == other.m_id);
  }

  bool ThingId::operator==(uint64_t const& other) const
  {
    return (this->m_id == other);
  }

  bool ThingId::operator!=(ThingId const& other) const
  {
    return !operator==(other);
  }

  bool ThingId::operator!=(uint64_t const& other) const
  {
    return !operator==(other);
  }

  std::string ThingId::to_string() const
  {
    return str(m_id);
  }

  friend std::ostream& operator<< (std::ostream& stream, ThingId const& thing)
  {
    stream << thing.m_id;

    return stream;
  }

  /// Call to get the thing associated with this.
  Thing* operator->(void);

  /// Call to get the thing associated with this.
  Thing const* operator->(void) const;

private:
  uint64_t m_id;
};

namespace std
{
  /// Hash functor for ThingId
  template<>
  struct hash<ThingId>
  {
    std::size_t operator()(const ThingId& key) const
    {
      std::size_t seed = 0;

      boost::hash_combine(seed, boost::hash_value(key.m_id));

      return seed;
    }
  };
}

#endif // THINGREF_H
