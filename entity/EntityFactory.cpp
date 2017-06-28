#include "stdafx.h"

#include "entity/EntityFactory.h"

#include "components/ComponentManager.h"
#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "maptile/MapTile.h"
#include "services/Service.h"
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

/// @todo Add optional parameter specifying material to use
EntityId EntityFactory::create(std::string category)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = S<IGameRules>().categoryData(category);

  auto& jsonComponents = data["components"];
  m_gameState.components().populate(new_id, jsonComponents);

  if (jsonComponents.count("materials") != 0)
  {
    auto& jsonMaterials = jsonComponents["materials"];

    if (jsonMaterials.is_array() && jsonMaterials.size() > 0)
    {
      /// @todo Choose one material randomly and apply it.
      ///       Right now, we just use the first one.
      std::string material = StringTransforms::squishWhitespace(jsonMaterials[0].get<std::string>());
      json& materialData = S<IGameRules>().categoryData(material, "materials");
      m_gameState.components().populate(new_id, materialData["components"]);
    }
  }

  if (m_initialized)
  {
    GAME.lua().callEntityFunction("on_create", new_id, {}, true);
  }

  return EntityId(new_id);
}

EntityId EntityFactory::createTileEntity(MapTile* mapTile, std::string category, std::string material)
{
  EntityId new_id = create(category);

  MapID map = mapTile->map();
  IntVec2 position = mapTile->getCoords();
  m_gameState.components().position[new_id].set(map, position);

  return EntityId(new_id);
}

EntityId EntityFactory::clone(EntityId original)
{
  auto& components = m_gameState.components();
  if (!components.category.existsFor(original))  return EntityId::Mu();

  EntityId newId = EntityId(m_nextEntityId);
  ++m_nextEntityId;

  components.clone(original, newId);

  return newId;
}


void EntityFactory::applyCategoryData(EntityId id, std::string subType, std::string name)
{
  if (id != EntityId::Mu())
  {
    json& data = S<IGameRules>().categoryData(name, subType);
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
    json& data = S<IGameRules>().categoryData(category);
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
  auto& components = m_gameState.components();

  if (id != EntityId::Mu())
  {
    if (components.category.existsFor(id))
    {
      components.erase(id);
    }
  }
  else
  {
    throw std::exception("Attempted to destroy Mu object!");
  }
}
