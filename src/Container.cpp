#include "Container.h"

#include <vector>

#include "Inventory.h"

struct Container::Impl
{
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
  // copy ctor
}

Container::~Container()
{
  //dtor
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
