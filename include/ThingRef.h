#ifndef THINGREF_H
#define THINGREF_H

#include "stdafx.h"

#include "LuaObject.h"

// Forward declaration
class ThingManager;

/// Definition of a ID/version pair.
class ThingId
{
public:
  struct
  {
    uint32_t id;
    uint32_t version;
  };

  ThingId::ThingId()
    :
    id{ 0 },
    version{ 0 }
  {}

  ThingId::ThingId(uint32_t id_, uint16_t version_)
    :
    id{ id_ },
    version{ version_ }
  {}

  ThingId::ThingId(uint64_t full_id_)
    :
    id{ full_id_ & 0xFFFFFFFF },
    version{ full_id_ >> 32 }
  {}

  ThingId::ThingId(lua_Integer full_id_)
    :
    id{ static_cast<uint64_t>(full_id_) & 0xFFFFFFFF },
    version{ static_cast<uint64_t>(full_id_) >> 32 }
  {}

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(id, version);
  }

  bool ThingId::operator<(ThingId const& other) const
  {
    return (this->to_uint64() < other.to_uint64());
  }

  bool ThingId::operator<=(ThingId const& other) const
  {
    return (this->to_uint64() <= other.to_uint64());
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
    return (this->to_uint64() == other.to_uint64());
  }

  bool ThingId::operator!=(ThingId const& other) const
  {
    return !operator==(other);
  }

  uint64_t ThingId::to_uint64() const
  {
    return (static_cast<uint64_t>(version) << 32) | (static_cast<uint64_t>(id));
  }

  std::string ThingId::to_string() const
  {
    return boost::lexical_cast<std::string>(id) + "." + boost::lexical_cast<std::string>(version);
  }

  friend std::ostream& operator<< (std::ostream& stream, ThingId const& thing)
  {
    stream << boost::lexical_cast<std::string>(thing.id)
      << "."
      << boost::lexical_cast<std::string>(thing.version);

    return stream;
  }
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

      boost::hash_combine(seed, boost::hash_value(key.version));
      boost::hash_combine(seed, boost::hash_value(key.id));

      return seed;
    }
  };
}

// Forward declarations
class Thing;

class ThingRef final
{
  friend class ThingManager;

public:

  /// Create a reference to Mu (the nothingness Thing).
  ThingRef();

  /// Create a weak reference from an ID.
  explicit ThingRef(ThingId const& id);

  ~ThingRef();

  /// Create a weak reference from another weak reference.
  ThingRef(ThingRef const& other);

  /// Assignment operator for weak references.
  ThingRef& operator=(ThingRef other);

  /// Conversion operator to lua_Integer.
  /// Makes it REALLY easy to pass things to Lua scripts.
  operator lua_Integer() const
  {
    return static_cast<lua_Integer>(get_id().to_uint64());
  }

  /// Serialization function.
  /// @todo This is SUPER DUPER INEFFICIENT as it will store the static data
  ///       once for EVERY SINGLE ThingRef. It really needs to be split out
  ///       into some sort of factory.
  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(m_id, S_largest_unused_id, S_deleted_ids);
  }

  bool operator<(ThingRef const& other) const;

  bool operator<=(ThingRef const& other) const;

  bool operator>(ThingRef const& other) const;

  bool operator>=(ThingRef const& other) const;

  /// Equivalence operator.
  bool operator==(ThingRef const& other) const;

  /// Non-equivalence operator.
  bool operator!=(ThingRef const& other) const;

  /// Call to get the thing associated with this.
  Thing* operator->(void);

  /// Call to get the ID associated with this.
  /// @warning This is ONLY exposed to allow for std::unordered_map hashing.
  ///          It should not be used in any other circumstance.
  ThingId get_id(void) const;

protected:
  /// Create a new ID.
  /// This ID is guaranteed to be unique from any other currently existing,
  /// or any that has previously existed in this game instance.
  static ThingId create();

  /// Destroy an ID that is no longer required.
  static void destroy(ThingId const& id);

  /// Swap function
  friend void swap(ThingRef& first, ThingRef& second);

private:
  /// The ID/version associated with this reference.
  ThingId m_id;

  /// Largest unused ID so far.
  static uint32_t S_largest_unused_id;

  /// Vector of ID/version pairs that have been deleted so far.
  static std::vector<ThingId> S_deleted_ids;
};

namespace std
{
  /// Hash functor for ThingRef
  template<>
  struct hash<ThingRef>
  {
    std::size_t operator()(const ThingRef& key) const
    {
      std::size_t seed = 0;

      ThingId id = key.get_id();

      boost::hash_combine(seed, boost::hash_value(id.version));
      boost::hash_combine(seed, boost::hash_value(id.id));

      return seed;
    }
  };
}

#endif // THINGREF_H
