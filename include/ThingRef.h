#ifndef THINGREF_H
#define THINGREF_H

#include "ThingManager.h"
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

#endif // THINGREF_H
