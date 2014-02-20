#include "Aggregate.h"

#include <boost/lexical_cast.hpp>

#include "Container.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
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

bool Aggregate::move_into(Container& new_location)
{
  Container& old_location = TF.get_container(this->get_location_id());

  if (is_movable())
  {
    if (new_location.can_contain(*this) == ActionResult::Success)
    {
      Inventory& new_inventory = new_location.get_inventory();

      ThingId similar_thing_id = ThingFactory::limbo_id;

      for (ThingMapById::const_iterator iter = new_inventory.by_id_cbegin();
           iter != new_inventory.by_id_cend();
           ++iter)
      {
        ThingId thing_id = (*iter).first;
        Thing const& thing = TF.get(thing_id);
        if (this->has_same_type_as(thing) &&
            this->has_same_qualities_as(thing))
        {
          TRACE("Found similar thing \"%u\" already present, combining!",
                static_cast<unsigned int>(thing_id));

          similar_thing_id = thing_id;
          break;
        }
      }

      if (similar_thing_id == ThingFactory::limbo_id)
      {
        // No similar things, just move as usual.
        if (new_inventory.add(this->get_id()))
        {
          old_location.get_inventory().remove(this->get_id());
          this->set_location_id(new_location.get_id());
          return true;
        }
      }
      else
      {
        // Similar thing found, combine them!
        Thing& similar_thing = TF.get(similar_thing_id);
        if (!isType(&similar_thing, Aggregate))
        {
          MAJOR_ERROR("Similar thing is not an Aggregate; "
                      "this should not be possible!");
        }
        else
        {
          Aggregate& similar_aggregate = dynamic_cast<Aggregate&>(similar_thing);
          unsigned int old_quantity = similar_aggregate.get_quantity();
          unsigned int new_quantity = old_quantity + this->get_quantity();

          similar_aggregate.set_quantity(new_quantity);
          this->set_quantity(0);
          old_location.get_inventory().remove(this->get_id());
          this->set_location_id(ThingFactory::limbo_id);
          return true;
        }
      }
    }
  }

  return false;
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
