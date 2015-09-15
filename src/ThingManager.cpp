#include "ThingManager.h"

#include "ErrorHandler.h"
#include "Lua.h"
#include "Thing.h"
#include "ThingRef.h"
#include "ThingMetadata.h"

#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <queue>
#include <string>
#include <unordered_map>

std::unique_ptr<ThingManager> ThingManager::instance_;

struct ThingManager::Impl
{
  /// ThingRef of the player.
  ThingRef player;

  /// Map of ThingMetaData.
  std::map< std::string, std::unique_ptr<ThingMetadata> > thing_metadata;

  /// Map of ThingIds to Things.
  std::map<ThingId, Thing*> thing_map;

  /// Object pool of Things that exist.
  boost::object_pool<Thing> thing_pool;

  /// ThingRef that means "nothing".
  ThingRef mu;
};

ThingManager::ThingManager()
  : pImpl(new Impl())
{
  // Register the Thing Lua functions.
  the_lua_instance.register_function("thing_get_intrinsic_flag", Thing::LUA_get_intrinsic_flag);
  the_lua_instance.register_function("thing_get_intrinsic_value", Thing::LUA_get_intrinsic_value);
  the_lua_instance.register_function("thing_get_intrinsic_string", Thing::LUA_get_intrinsic_string);
  the_lua_instance.register_function("thing_get_property_flag", Thing::LUA_get_property_flag);
  the_lua_instance.register_function("thing_get_property_value", Thing::LUA_get_property_value);
  the_lua_instance.register_function("thing_get_property_string", Thing::LUA_get_property_string);
  the_lua_instance.register_function("thing_set_property_flag", Thing::LUA_set_property_flag);
  the_lua_instance.register_function("thing_set_property_value", Thing::LUA_set_property_value);
  the_lua_instance.register_function("thing_set_property_string", Thing::LUA_set_property_string);
}

ThingManager::~ThingManager()
{
}

void ThingManager::initialize()
{
  // Create the "nothingness" object.
  pImpl->mu = create("Mu");
}

ThingManager& ThingManager::instance()
{
  if (ThingManager::instance_ == nullptr)
  {
    ThingManager::instance_.reset(new ThingManager());
    ThingManager::instance_->initialize();
  }

  return *(ThingManager::instance_.get());
}

ThingRef ThingManager::create(std::string type)
{
  if (pImpl->thing_metadata.count(type) == 0)
  {
    pImpl->thing_metadata[type].reset(new ThingMetadata(type));
  }

  ThingId new_id = ThingRef::create();
  Thing* new_thing = pImpl->thing_pool.construct(type, ThingRef(new_id));
  pImpl->thing_map[new_id] = new_thing;

  // Temporary test of Lua call
  pImpl->thing_metadata[type]->call_lua_function("on_create", new_id);

  return ThingRef(new_id);
}

ThingRef ThingManager::create_floor(MapTile* map_tile)
{
  if (pImpl->thing_metadata.count("Floor") == 0)
  {
    pImpl->thing_metadata["Floor"].reset(new ThingMetadata("Floor"));
  }

  ThingId new_id = ThingRef::create();
  Thing* new_thing = pImpl->thing_pool.construct(map_tile, ThingRef(new_id));
  pImpl->thing_map[new_id] = new_thing;

  return ThingRef(new_id);
}

ThingRef ThingManager::clone(ThingRef original_ref)
{
  if (TM.exists(original_ref) == false) return pImpl->mu;
  Thing* original_thing = TM.get_ptr(original_ref.m_id);

  ThingId new_id = ThingRef::create();
  Thing* new_thing = pImpl->thing_pool.construct(*original_thing);
  pImpl->thing_map[new_id] = new_thing;

  return ThingRef(new_id);
}

ThingMetadata& ThingManager::get_metadata(std::string type)
{
  if (pImpl->thing_metadata.count(type) == 0)
  {
    pImpl->thing_metadata[type].reset(new ThingMetadata(type));
  }

  return *(pImpl->thing_metadata[type]);
}

void ThingManager::destroy(ThingRef ref)
{
  if (ref != pImpl->mu)
  {
    if (pImpl->thing_map.count(ref.m_id) != 0)
    {
      Thing* old_thing = pImpl->thing_map[ref.m_id];
      pImpl->thing_pool.destroy(old_thing);
      pImpl->thing_map.erase(ref.m_id);
    }
  }
  else
  {
    throw std::exception("Attempted to destroy Mu object!");
  }
}

bool ThingManager::exists(ThingRef ref)
{
  return (pImpl->thing_map.count(ref.m_id) != 0);
}

Thing* ThingManager::get_ptr(ThingId id)
{
  try
  {
    return pImpl->thing_map.at(id);
  }
  catch (std::out_of_range&)
  {
    MAJOR_ERROR("Tried to get thing %s which does not exist", boost::lexical_cast<std::string>(id).c_str());
    return pImpl->thing_map[pImpl->mu.m_id];
  }
}

Thing const* ThingManager::get_ptr(ThingId id) const
{
  try
  {
    return pImpl->thing_map.at(id);
  }
  catch (std::out_of_range&)
  {
    MAJOR_ERROR("Tried to get thing %s which does not exist", boost::lexical_cast<std::string>(id).c_str());
    return pImpl->thing_map[pImpl->mu.m_id];
  }
}


bool ThingManager::set_player(ThingRef ref)
{
  ASSERT_CONDITION(ref != pImpl->mu);

  pImpl->player = ref;
  return true;
}

ThingRef ThingManager::get_player() const
{
  return pImpl->player;
}

ThingRef ThingManager::get_mu() const
{
  return pImpl->mu;
}