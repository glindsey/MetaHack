#include "Aggregate.h"

#include <boost/lexical_cast.hpp>

#include "Container.h"
#include "Gender.h"
#include "ThingFactory.h"

struct Aggregate::Impl
{
  unsigned int quantity;
};

Aggregate::Aggregate() :
  Thing(), impl(new Impl())
{
  impl->quantity = 1;
}

Aggregate::Aggregate(Aggregate const& original) :
  Thing(original), impl(new Impl())
{
  impl->quantity = original.get_quantity();
}

Aggregate::Aggregate(Aggregate& original, unsigned int number) :
  Thing(original), impl(new Impl())
{
  unsigned int original_number = original.get_quantity();

  if (number > original_number) number = original_number;

  impl->quantity = number;
  original.set_quantity(original_number - number);
}

unsigned int Aggregate::get_quantity() const
{
  return impl->quantity;
}

void Aggregate::set_quantity(unsigned int quantity)
{
  impl->quantity = quantity;
}

int Aggregate::get_size() const
{
  return impl->quantity * get_single_size();
}

int Aggregate::get_mass() const
{
  return impl->quantity * get_single_mass();
}

std::string Aggregate::get_name() const
{
  // If the thing is YOU, use YOU.
  if (get_id() == TF.get_player_id())
  {
    return "you";
  }

  std::string name;
  Container& owner = TF.get_container(get_owner_id());

  // If the Thing has a proper name, use that.
  if (!get_proper_name().empty())
  {
    if (owner.is_entity())
    {
      name = owner.get_possessive() + " ";
    }

    name += get_proper_name();
  }
  else
  {
    if (owner.is_entity())
    {
      name = owner.get_possessive() + " ";
    }
    else if (get_quantity() == 1)
    {
      name = "the " + get_description();
    }
    else
    {
      name = boost::lexical_cast<std::string>(get_quantity()) + " " +
             get_plural();
    }
  }

  return name;
}

std::string Aggregate::get_indef_name() const
{
  // If the thing is YOU, use YOU.
  if (get_id() == TF.get_player_id())
  {
    return "you";
  }

  std::string name;

  // If the Thing has a proper name, use that.
  if (!get_proper_name().empty())
  {
    name = get_proper_name();
  }
  else
  {
    if (get_quantity() == 1)
    {
      std::string description = get_description();
      name = getIndefArt(description) + " " + description;
    }
    else
    {
      name = boost::lexical_cast<std::string>(get_quantity()) + " " +
            get_plural();
    }
  }

  return name;
}

Aggregate::~Aggregate()
{
  //dtor
}
