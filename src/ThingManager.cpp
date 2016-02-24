#include "ThingManager.h"

#include "App.h"
#include "ErrorHandler.h"
#include "GameState.h"
#include "Lua.h"
#include "LuaThingFunctions.h"
#include "Metadata.h"
#include "Thing.h"
#include "ThingRef.h"

#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cinttypes>
#include <queue>
#include <string>

ThingManager::ThingManager()
{
  // Register the Thing Lua functions.
  LuaThingFunctions::register_functions();

  // Create the "nothingness" object.
  ThingRef mu = create("Mu");
  if (mu.get_id().to_uint64() != 0)
  {
    FATAL_ERROR("Mu's ID is %" PRIu64 " instead of zero!", mu.get_id().to_uint64());
  }
}

ThingManager::~ThingManager()
{
}

ThingRef ThingManager::create(std::string type)
{
  ThingId new_id = ThingRef::create();
  ThingRef new_ref = ThingRef(new_id);
  Metadata& metadata = MDC::get_collection("thing").get(type);

  std::unique_ptr<Thing> new_thing{ new Thing{ metadata, new_ref} };
  m_thing_map[new_id] = std::move(new_thing);
  m_thing_map[new_id]->call_lua_function("on_create", {});

  return ThingRef(new_id);
}

ThingRef ThingManager::create_tile_contents(MapTile* map_tile)
{
  ThingId new_id = ThingRef::create();
  ThingRef new_ref = ThingRef(new_id);
  Metadata& metadata = MDC::get_collection("thing").get("TileContents");

  std::unique_ptr<Thing> new_thing{ new Thing { map_tile, metadata, new_ref } };
  m_thing_map[new_id] = std::move(new_thing);

  return ThingRef(new_id);
}

ThingRef ThingManager::clone(ThingRef original_ref)
{
  if (this->exists(original_ref) == false) return get_mu();
  Thing& original_thing = this->get(original_ref.m_id);

  ThingId new_id = ThingRef::create();
  ThingRef new_ref = ThingRef(new_id);

  std::unique_ptr<Thing> new_thing{ new Thing { original_thing, new_ref} };
  m_thing_map[new_id] = std::move(new_thing);

  return ThingRef(new_id);
}

void ThingManager::destroy(ThingRef ref)
{
  if (ref != get_mu())
  {
    if (m_thing_map.count(ref.m_id) != 0)
    {
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

Thing& ThingManager::get(ThingId id)
{
  try
  {
    return *(m_thing_map.at(id).get());
  }
  catch (std::out_of_range&)
  {
    MAJOR_ERROR("Tried to get thing %s which does not exist", boost::lexical_cast<std::string>(id).c_str());
    return *(m_thing_map[get_mu().m_id].get());
  }
}

Thing const& ThingManager::get(ThingId id) const
{
  try
  {
    return *(m_thing_map.at(id).get());
  }
  catch (std::out_of_range&)
  {
    MAJOR_ERROR("Tried to get thing %s which does not exist", boost::lexical_cast<std::string>(id).c_str());
    return *(m_thing_map.at(get_mu().m_id).get());
  }
}

ThingRef MU
{
  return ThingRef();
}