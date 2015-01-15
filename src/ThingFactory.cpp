#include "ThingFactory.h"

#include "Entity.h"
#include "ErrorHandler.h"
#include "Inventory.h"
#include "MapTile.h"

#include <string>
#include <unordered_map>

std::unique_ptr<ThingFactory> ThingFactory::instance_;

struct ThingFactory::Impl
{
  std::shared_ptr<Entity> player;
  std::unordered_map<std::string, ThingCreator> thing_types;
};

ThingFactory::ThingFactory()
  : impl(new Impl())
{
}

ThingFactory::~ThingFactory()
{
}

void ThingFactory::initialize()
{
}

ThingFactory& ThingFactory::instance()
{
  if (ThingFactory::instance_ == nullptr)
  {
    ThingFactory::instance_.reset(new ThingFactory());
    ThingFactory::instance_->initialize();
  }

  return *(ThingFactory::instance_.get());
}

std::shared_ptr<Thing> ThingFactory::create(char const* name)
{
  std::shared_ptr<Thing> new_thing;

  if (impl->thing_types.count(name) == 0)
  {
    FATAL_ERROR("Tried to create unregistered thing of type %s; "
                "maybe class is missing a prototype object?", name);
    return std::shared_ptr<Thing>();
  }

  ThingCreator create = impl->thing_types[name];
  new_thing = std::static_pointer_cast<Thing>((*create)());

  return new_thing;
}

void ThingFactory::register_by_typename(std::string name,
                                        ThingCreator creator)
{
  TRACE("Registered type %s", name.c_str());
  impl->thing_types[name] = creator;
}

bool ThingFactory::set_player(std::shared_ptr<Entity> entity)
{
  ASSERT_CONDITION(entity);

  impl->player = entity;
  return true;
}

std::shared_ptr<Entity> ThingFactory::get_player() const
{
  return impl->player;
}
