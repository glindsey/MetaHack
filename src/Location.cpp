#include "Location.h"

#include "Inventory.h"
#include "Thing.h"

struct Location::Impl
{
  std::unique_ptr<Inventory> inventory_;
};

Location::Location()
  : impl(new Impl())
{
}

Location::~Location()
{
}

Inventory& Location::inventory()
{
  if (impl->inventory_.get() == nullptr)
  {
    impl->inventory_.reset(new Inventory(*this));
  }

  return *(impl->inventory_.get());
}

bool Location::canContain(Thing* thing) const
{
  return true;
}
