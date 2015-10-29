#include "ThingRef.h"
#include "Thing.h"
#include "ThingManager.h"

// Static declarations
uint32_t ThingRef::S_largest_unused_id = 0;
std::vector<ThingId> ThingRef::S_deleted_ids;

ThingRef::ThingRef()
{
  this->m_id = ThingId(0, 0);
}

ThingRef::ThingRef(ThingId const& id)
{
  this->m_id = id;
}

ThingRef::~ThingRef()
{
}

/// Create a weak reference from another weak reference.
ThingRef::ThingRef(ThingRef const& other)
{
  this->m_id = other.m_id;
}

/// Assignment operator for weak references.
ThingRef& ThingRef::operator=(ThingRef other)
{
  swap(*this, other);
  return *this;
}

bool ThingRef::operator<(ThingRef const& other) const
{
  return (this->m_id < other.m_id);
}

bool ThingRef::operator<=(ThingRef const& other) const
{
  return (this->m_id <= other.m_id);
}

bool ThingRef::operator>(ThingRef const& other) const
{
  return !operator<=(other);
}

bool ThingRef::operator>=(ThingRef const& other) const
{
  return !operator<(other);
}

bool ThingRef::operator==(ThingRef const& other) const
{
	return (this->m_id == other.m_id);
}

bool ThingRef::operator!=(ThingRef const& other) const
{
  return !operator==(other);
}

Thing* ThingRef::operator->()
{
	return TM.get_ptr(m_id);
}

ThingId ThingRef::get_id() const
{
  return this->m_id;
}

ThingId ThingRef::create()
{
	ThingId new_id;

	if (S_deleted_ids.size() != 0)
	{
		ThingId last_deleted = S_deleted_ids.back();
		S_deleted_ids.pop_back();
		++(last_deleted.version);
		new_id = last_deleted;
	}
	else
	{
		new_id.id = S_largest_unused_id;
		new_id.version = 0;
		++(S_largest_unused_id);
	}

	return new_id;
}

void ThingRef::destroy(ThingId const& id)
{
	S_deleted_ids.push_back(id);
}

/// Swap function
void swap(ThingRef& first, ThingRef& second)
{
  using std::swap;
  swap(first.m_id, second.m_id);
}
