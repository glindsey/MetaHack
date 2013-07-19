#include "Limbo.h"

Limbo::Limbo() :
  Container()
{
  set_single_size(0);
  set_single_mass(0);
}

Limbo::~Limbo() {}

std::string Limbo::get_description() const
{
  return "nothingness";
}
