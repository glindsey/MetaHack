#include "Printable.h"

#include <boost/core/demangle.hpp>
#include <typeinfo>

void Printable::printToStream(std::ostream& o) const
{
   o << boost::core::demangle(typeid(*this).name()) << ": " << this;
}

std::ostream& operator<<(std::ostream& o, const Printable& object)
{
  o << "<";
  object.printToStream(o);
  o << ">";
  return o;
}
