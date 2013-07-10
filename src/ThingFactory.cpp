#include "ThingFactory.h"

#include "Entity.h"
#include "ErrorHandler.h"
#include "Inventory.h"
#include "IsType.h"
#include "Limbo.h"
#include "MapTile.h"

#include <string>
#include <unordered_map>

const ThingId ThingFactory::limbo_id = static_cast<ThingId>(0);

std::unique_ptr<ThingFactory> ThingFactory::instance_;

struct ThingFactory::Impl
{
  std::vector<Thing*> things;

  Thing* limbo_thing_ptr;
  Entity* player_ptr;
  ThingId player_id;

  std::unordered_map<std::string, ThingCreator> thing_types;
};

ThingFactory::ThingFactory()
  : impl(new Impl())
{
}

ThingFactory::~ThingFactory()
{
  // Clean up any Things that haven't yet been deleted.
  // TODO: may want to speed this up using a memory pool or something
  for (unsigned int num = 0; num < impl->things.size(); ++num)
  {
    if (impl->things[num] != nullptr)
    {
      delete impl->things[num];
    }
  }
}

void ThingFactory::initialize()
{
  // Create and add Limbo to the list.
  Thing* new_limbo = Limbo::create();
  new_limbo->set_id(limbo_id);
  new_limbo->set_location_id(limbo_id);

  impl->things.push_back(new_limbo);
  impl->limbo_thing_ptr = impl->things[limbo_id];
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

Thing& ThingFactory::get(ThingId thing_id)
{
  if ((static_cast<unsigned int>(thing_id) >= impl->things.size()) ||
      (impl->things[thing_id] == nullptr))
  {
    return *(impl->limbo_thing_ptr);
  }
  else
  {
    return *(impl->things[thing_id]);
  }
}

bool ThingFactory::is_a_tile(ThingId thing_id)
{
  if ((static_cast<unsigned int>(thing_id) >= impl->things.size()) ||
      (impl->things[thing_id] == nullptr))
  {
    FATAL_ERROR("MapTile with ThingId %u is missing!",
                static_cast<unsigned int>(thing_id));
  }

  return isType(impl->things[thing_id], MapTile);
}

MapTile& ThingFactory::get_tile(ThingId thing_id)
{
  if ((static_cast<unsigned int>(thing_id) >= impl->things.size()) ||
      (impl->things[thing_id] == nullptr))
  {
    FATAL_ERROR("MapTile with ThingId %u is missing!",
                static_cast<unsigned int>(thing_id));
  }
  else if (!isType(impl->things[thing_id], MapTile))
  {
    FATAL_ERROR("Requested ThingId %u, but that isn't a MapTile!",
                static_cast<unsigned int>(thing_id));
  }

  return dynamic_cast<MapTile&>(*(impl->things[thing_id]));
}

ThingId ThingFactory::create(char const* name)
{
  ThingId new_thing_id = static_cast<ThingId>(impl->things.size());
  Thing* new_thing;

  if (impl->thing_types.count(name) == 0)
  {
    FATAL_ERROR("Tried to create unregistered thing of type %s; "
                "maybe class is missing a prototype object?", name);
    return limbo_id;
  }

  ThingCreator create = impl->thing_types[name];
  new_thing = (*create)();
  new_thing->set_id(new_thing_id);
  impl->things.push_back(new_thing);

  // Add the thing to Limbo's inventory.
  get_limbo().get_inventory().add(new_thing_id);

  return new_thing_id;
}

ThingId ThingFactory::create_tile(MapId mapId, int x, int y)
{
  ThingId new_thing_id = static_cast<ThingId>(impl->things.size());

  MapTile* new_tile = new MapTile(mapId, x, y);

  new_tile->set_id(new_thing_id);
  new_tile->set_location_id(new_thing_id);
  impl->things.push_back(new_tile);

  // Add the thing to Limbo's inventory.
  get_limbo().get_inventory().add(new_thing_id);

  return new_thing_id;
}

bool ThingFactory::destroy(ThingId thing_id)
{
  // Can't destroy limbo.
  if (thing_id == limbo_id)
  {
    MAJOR_ERROR("Attempted to destroy Limbo!");
    return false;
  }

  // Can't destroy the player entity.
  if (thing_id == impl->player_id)
  {
    MAJOR_ERROR("Attempted to destroy the player entity!");
    return false;
  }

  if (impl->things[thing_id])
  {
    delete impl->things[thing_id];
    impl->things[thing_id] = nullptr;
    return true;
  }
  else
  {
    return false;
  }
}

bool ThingFactory::set_player_id(ThingId thing_id)
{
  if (impl->things[thing_id] != nullptr)
  {
    Thing* thing = impl->things[thing_id];
    if (isType(thing, Entity))
    {
      impl->player_id = thing_id;
      impl->player_ptr = dynamic_cast<Entity*>(thing);
      return true;
    }
    else
    {
      FATAL_ERROR("Cannot set player ID to a non-Entity!");
      return false;
    }
  }
  else
  {
    FATAL_ERROR("Cannot set player ID to a null Thing!");
    return false;
  }
}

ThingId ThingFactory::get_player_id()
{
  return impl->player_id;
}

Entity& ThingFactory::get_player()
{
  return *(impl->player_ptr);
}

Thing& ThingFactory::get_limbo()
{
  return *(impl->limbo_thing_ptr);
}

void ThingFactory::register_by_typename(char const* name,
                                        ThingCreator creator)
{
  TRACE("Registered type %s", name);
  impl->thing_types[name] = creator;
}
