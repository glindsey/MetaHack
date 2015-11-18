#include "ThingManager.h"

#include "App.h"
#include "ErrorHandler.h"
#include "Lua.h"
#include "Metadata.h"
#include "Thing.h"
#include "ThingRef.h"

#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <queue>
#include <string>

std::unique_ptr<ThingManager> ThingManager::instance_;

ThingManager::ThingManager()
{
  // Register the Thing Lua functions.
  the_lua_instance.register_function("thing_create", Thing::LUA_thing_create);
  the_lua_instance.register_function("thing_destroy", Thing::LUA_thing_create);
  the_lua_instance.register_function("thing_get_player", Thing::LUA_thing_get_player);
  the_lua_instance.register_function("thing_get_coords", Thing::LUA_thing_get_coords);
  the_lua_instance.register_function("thing_get_type", Thing::LUA_thing_get_type);
  the_lua_instance.register_function("thing_get_property_flag", Thing::LUA_thing_get_property_flag);
  the_lua_instance.register_function("thing_get_property_value", Thing::LUA_thing_get_property_value);
  the_lua_instance.register_function("thing_get_property_string", Thing::LUA_thing_get_property_string);
  the_lua_instance.register_function("thing_set_property_flag", Thing::LUA_thing_set_property_flag);
  the_lua_instance.register_function("thing_set_property_value", Thing::LUA_thing_set_property_value);
  the_lua_instance.register_function("thing_set_property_string", Thing::LUA_thing_set_property_string);
  the_lua_instance.register_function("thing_move_into", Thing::LUA_thing_move_into);
}

ThingManager::~ThingManager()
{
}

void ThingManager::initialize()
{
  // Create the "nothingness" object.
  m_mu = create("Mu");
}

ThingManager& ThingManager::instance()
{
  if (ThingManager::instance_ == nullptr)
  {
    ThingManager::instance_.reset(NEW ThingManager());
    ThingManager::instance_->initialize();
  }

  return *(ThingManager::instance_.get());
}

ThingRef ThingManager::create(std::string type)
{
  ThingId new_id = ThingRef::create();
  ThingRef new_ref = ThingRef(new_id);
  Metadata& metadata = MDC::get_collection("thing").get(type);

  Thing* new_thing = m_thing_pool.construct(boost::ref(metadata), new_ref);
  m_thing_map[new_id] = new_thing;

  // Temporary test of Lua call
  new_thing->call_lua_function("on_create", {});

  return ThingRef(new_id);
}

ThingRef ThingManager::create_floor(MapTile* map_tile)
{
  ThingId new_id = ThingRef::create();
  ThingRef new_ref = ThingRef(new_id);
  Metadata& metadata = MDC::get_collection("thing").get("Floor");

  Thing* new_thing = m_thing_pool.construct(map_tile, boost::ref(metadata), new_ref);
  m_thing_map[new_id] = new_thing;

  return ThingRef(new_id);
}

ThingRef ThingManager::clone(ThingRef original_ref)
{
  if (TM.exists(original_ref) == false) return m_mu;
  Thing* original_thing = TM.get_ptr(original_ref.m_id);

  ThingId new_id = ThingRef::create();
  ThingRef new_ref = ThingRef(new_id);

  Thing* new_thing = m_thing_pool.construct(*original_thing, new_ref);
  m_thing_map[new_id] = new_thing;

  return ThingRef(new_id);
}

void ThingManager::destroy(ThingRef ref)
{
  if (ref != m_mu)
  {
    if (m_thing_map.count(ref.m_id) != 0)
    {
      Thing* old_thing = m_thing_map[ref.m_id];
      m_thing_pool.destroy(old_thing);
      m_thing_map.erase(ref.m_id);
    }
  }
  else
  {
    throw std::exception("Attempted to destroy Mu object!");
  }
}

bool ThingManager::exists(ThingRef ref)
{
  return (m_thing_map.count(ref.m_id) != 0);
}

Thing* ThingManager::get_ptr(ThingId id)
{
  try
  {
    return m_thing_map.at(id);
  }
  catch (std::out_of_range&)
  {
    MAJOR_ERROR("Tried to get thing %s which does not exist", boost::lexical_cast<std::string>(id).c_str());
    return m_thing_map[m_mu.m_id];
  }
}

Thing const* ThingManager::get_ptr(ThingId id) const
{
  try
  {
    return m_thing_map.at(id);
  }
  catch (std::out_of_range&)
  {
    MAJOR_ERROR("Tried to get thing %s which does not exist", boost::lexical_cast<std::string>(id).c_str());
    return m_thing_map.at(m_mu.m_id);
  }
}


bool ThingManager::set_player(ThingRef ref)
{
  ASSERT_CONDITION(ref != m_mu);

  m_player = ref;
  return true;
}

ThingRef ThingManager::get_player() const
{
  return m_player;
}

ThingRef ThingManager::get_mu() const
{
  return m_mu;
}