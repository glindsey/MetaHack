#include "Printable.h"

#include <boost/core/demangle.hpp>
#include <typeinfo>

void Printable::printToStream(std::ostream& o) const
{
   o << toString();
}

std::string Printable::toString() const
{
  std::stringstream ss;
  ss << boost::core::demangle(typeid(*this).name()) << ": " << this;
  return ss.str();
}

std::ostream& operator<<(std::ostream& o, const Printable& object)
{
  o << "<";
  object.printToStream(o);
  o << ">";
  return o;
}
