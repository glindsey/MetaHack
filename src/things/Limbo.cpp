#include "Limbo.h"

Limbo::Limbo() :
  Thing()
{
  set_size(0);
  set_mass(0);
}

Limbo::~Limbo() {}

std::string Limbo::get_description() const
{
  return "nothingness";
}
