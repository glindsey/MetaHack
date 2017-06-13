#include "stdafx.h"

#include "entity/EntityFactory.h"

#include "components/ComponentManager.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "Service.h"
#include "services/IGameRules.h"

EntityFactory::EntityFactory(GameState& gameState) :
  m_gameState{ gameState }
{
  // Create the "nothingness" object.
  EntityId mu = create("Mu");
  Assert("EntityFactory", (mu == 0ULL), "Mu's ID is " << mu << " instead of zero!");

  m_initialized = true;
}

EntityFactory::~EntityFactory()
{
}

EntityId EntityFactory::create(std::string category)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = Service<IGameRules>::get().category(category);

  std::unique_ptr<Entity> new_thing{ new Entity{ m_gameState, category, new_id } };

  auto& jsonComponents = data["components"];
  m_thing_map[new_id] = std::move(new_thing);
  m_gameState.components().populate(new_id, jsonComponents);

  if (jsonComponents.count("materials") != 0)
  {
    auto& jsonMaterials = jsonComponents["materials"];

    if (jsonMaterials.is_array() && jsonMaterials.size() > 0)
    {
      /// @todo Choose one material randomly and apply it.
      ///       Right now, we just use the first one.
      std::string material = StringTransforms::squishWhitespace(jsonMaterials[0].get<std::string>());
      json& materialData = Service<IGameRules>::get().category(material, "materials");
      m_gameState.components().populate(new_id, materialData["components"]);
    }
  }

  if (m_initialized)
  {
    /// @todo Re-implement me
    m_thing_map[new_id]->call_lua_function("on_create", {}, true);
  }

  return EntityId(new_id);
}

EntityId EntityFactory::createTileContents(MapTile* map_tile)
{
  EntityId new_id = create("TileContents");

  MapID map = map_tile->map();
  IntVec2 position = map_tile->getCoords();
  m_gameState.components().position[new_id].set(map, position);

  return EntityId(new_id);
}

EntityId EntityFactory::clone(EntityId original)
{
  if (this->exists(original) == false) return EntityId::Mu();
  Entity& original_thing = this->get(original);

  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;

  std::unique_ptr<Entity> new_thing{ new Entity { original_thing, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  return EntityId(new_id);
}


void EntityFactory::applyCategoryData(EntityId id, std::string subType, std::string name)
{
  if (id != EntityId::Mu())
  {
    json& data = Service<IGameRules>::get().category(name, subType);
    auto& jsonComponents = data["components"];
    m_gameState.components().populate(id, jsonComponents);
  }
  else
  {
    throw std::exception("Attempted to apply category data to Mu object!");
  }
}

void EntityFactory::morph(EntityId id, std::string category)
{
  if (id != EntityId::Mu())
  {
    json& data = Service<IGameRules>::get().category(category);
    auto& jsonComponents = data["components"];
    m_gameState.components().erase(id);
    m_gameState.components().populate(id, jsonComponents);
  }
  else
  {
    throw std::exception("Attempted to morph Mu object!");
  }
}

void EntityFactory::destroy(EntityId id)
{
  if (id != EntityId::Mu())
  {
    if (m_thing_map.count(id) != 0)
    {
      m_thing_map.erase(id);
      m_gameState.components().erase(id);
    }
  }
  else
  {
    throw std::exception("Attempted to destroy Mu object!");
  }
}

bool EntityFactory::exists(EntityId id)
{
  return (m_thing_map.count(id) != 0);
}

Entity& EntityFactory::get(EntityId id)
{
  try
  {
    return *(m_thing_map.at(id));
  }
  catch (std::out_of_range&)
  {
    CLOG(WARNING, "Entity") << "Tried to get entity " << id << " which does not exist";
    return *(m_thing_map[EntityId::Mu()]);
  }
}

Entity const& EntityFactory::get(EntityId id) const
{
  try
  {
    return *(m_thing_map.at(id));
  }
  catch (std::out_of_range&)
  {
    CLOG(WARNING, "Entity") << "Tried to get entity " << id << " which does not exist";
    return *(m_thing_map.at(EntityId::Mu()));
  }
}