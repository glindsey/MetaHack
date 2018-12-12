#include "Serializable.h"

#include <boost/core/demangle.hpp>
#include <typeinfo>

void Serializable::serialize(std::ostream& o) const
{
   o << boost::core::demangle(typeid(*this).name()) << ": " << this;
}

std::ostream& operator<<(std::ostream& o, const Serializable& object)
{
  o << "<";
  object.serialize(o);
  o << ">";
  return o;
}
