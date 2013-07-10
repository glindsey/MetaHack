#include "ThingId.h"

ThingId ThingId::nilThing_(boost::uuids::nil_generator());

ThingId::ThingId()
{
  id_ = boost::uuids::random_generator();
}

ThingId::ThingId(boost::uuids::uuid id)
{
  id_ = id;
}

~ThingId::ThingId() {}

ThingId::ThingId& operator=(const ThingId& rhs)
{
  if (this != &rhs)
  {
    this->id_ = rhs.id_;
  }

  return *this;
}

bool ThingId::operator==(const ThingId& other) const
{
  return (this->id_ == other.id_);
}

bool ThingId::operator!=(const ThingId& other) const
{
  return !(*this == other);
}

bool ThingId::operator<(const ThingId& other) const
{
  return (this->id_ < other.id_);
}

bool ThingId::operator>(const ThingId& other) const
{
  return (this->id_ > other.id_);
}

bool ThingId::operator<=(const ThingId& other) const
{
  return (*this < other) || (*this == other);
}

bool ThingId::operator>=(const ThingId& other) const
{
  return (*this > other) || (*this == other);
}

static ThingId ThingId::nil()
{
  return ThingId::nilThing_;
}


