#ifndef THINGREF_H
#define THINGREF_H

#include "ThingManager.h"
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <memory>
#include <vector>

/// Definition of a ID/version pair.
class ThingId
{
public:
  union
  {
    uint64_t full_id;
    struct
    {
      uint32_t id;
      uint32_t version;
    };
  };

  ThingId::ThingId()
    : id(0), version(0) {}

  ThingId::ThingId(uint32_t id_, uint16_t version_)
    : id(id_), version(version_) {}

  ThingId::ThingId(uint64_t full_id_)
    : full_id(full_id_) {}

  bool ThingId::operator<(ThingId const& other) const
  {
    return (this->full_id < other.full_id);
  }

  bool ThingId::operator<=(ThingId const& other) const
  {
    return (this->full_id <= other.full_id);
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
		return (this->full_id == other.full_id);
	}

  bool ThingId::operator!=(ThingId const& other) const
  {
    return !operator==(other);
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

class ThingRef
{
	friend class ThingManager;

public:

  ThingRef();

  /// Create a weak reference from an ID.
	explicit ThingRef(ThingId const& id);

	virtual ~ThingRef();

	/// Create a weak reference from another weak reference.
	ThingRef(ThingRef const& other);

	/// Assignment operator for weak references.
	ThingRef& operator=(ThingRef other);

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
