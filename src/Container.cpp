#include "Container.h"

#include <vector>

#include "Inventory.h"
#include "ThingFactory.h"

struct Container::Impl
{
  std::string proper_name;
  Inventory inventory;
  int inventory_size;
};

Container::Container() :
  Thing(), impl(new Impl())
{
  impl->inventory_size = -1;
}

Container::Container(Container const& original) :
  Thing(original), impl(new Impl())
{
  impl->proper_name = original.get_proper_name();
}

Container::~Container()
{
  //dtor
}

std::string Container::get_proper_name() const
{
  return impl->proper_name;
}

void Container::set_proper_name(std::string name)
{
  impl->proper_name = name;
}

std::string Container::get_name() const
{
  // If the thing is YOU, use YOU.
  if (this->get_id() == TF.get_player_id())
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
    name = Thing::get_name();
  }

  return name;
}

std::string Container::get_indef_name() const
{
  // If the thing is YOU, use YOU.
  if (this->get_id() == TF.get_player_id())
  {
    return "you";
  }

  // If the Thing has a proper name, use that.
  if (!get_proper_name().empty())
  {
    return get_proper_name();
  }
  else
  {
    return Thing::get_indef_name();
  }
}

Inventory& Container::get_inventory()
{
  return impl->inventory;
}

int const Container::get_inventory_size() const
{
  return impl->inventory_size;
}

void Container::set_inventory_size(int number)
{
  /// @todo Don't allow shrinking below current inventory size, or perhaps
  ///       automatically pop things out that don't fit!
  impl->inventory_size = number;
}

void Container::gather_thing_ids(std::vector<ThingId>& ids)
{
  // Get all IDs in the Thing's inventory first.
  Inventory& inv = this->get_inventory();

  ThingMapById::iterator iter = inv.by_id_begin();

  while (iter != inv.by_id_end())
  {
    ids.push_back((*iter).first);
    ++iter;
  }

  // Then add this.
  ids.push_back(this->get_id());
}
