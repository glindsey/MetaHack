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
  bool is_owned = owner.is_entity();
  unsigned int quantity = this->get_quantity();

  if ((quantity == 1) && (is_owned))
  {
    name = owner.get_possessive() + " " + get_description();
  }
  else if ((quantity == 1) && (!(is_owned)))
  {
    name = "the " + get_description();
  }
  else if ((quantity > 1) && (is_owned))
  {
    name = owner.get_possessive() + " " +
           boost::lexical_cast<std::string>(get_quantity()) + " " +
           get_plural();
  }
  else if ((quantity > 1) && (!is_owned))
  {
    name = boost::lexical_cast<std::string>(get_quantity()) + " " +
           get_plural();
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

  return name;
}

Aggregate::~Aggregate()
{
  //dtor
}
