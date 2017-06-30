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
  EntityId nothingness = create("Void");
  Assert("EntityFactory", (nothingness == 0ULL), "Void's ID is " << nothingness << " instead of zero!");

  m_initialized = true;
}

EntityFactory::~EntityFactory()
{
}

EntityId EntityFactory::create(std::string category, std::string material)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = S<IGameRules>().categoryData(category);

  auto& jsonComponents = data["components"];
  m_gameState.components().populate(new_id, jsonComponents);

  if (material.empty() && jsonComponents.count("materials") != 0)
  {
    auto& jsonMaterials = jsonComponents["materials"];

    if (jsonMaterials.is_array() && jsonMaterials.size() > 0)
    {
      /// @todo Choose one material randomly.
      ///       Right now, we just use the first one.
      material = StringTransforms::squishWhitespace(jsonMaterials[0].get<std::string>());
    }
  }

  if (material.empty())
  {
    json& materialData = S<IGameRules>().categoryData(material, "materials");
    m_gameState.components().populate(new_id, materialData["components"]);
  }

  if (m_initialized)
  {
    GAME.lua().callEntityFunction("on_create", new_id, {}, true);
  }

  return EntityId(new_id);
}

EntityId EntityFactory::createTileEntity(MapTile* mapTile, std::string category, std::string material)
{
  EntityId new_id = create(category, material);

  MapID map = mapTile->map();
  IntVec2 position = mapTile->getCoords();
  m_gameState.components().position[new_id].set(map, position);

  return EntityId(new_id);
}

EntityId EntityFactory::clone(EntityId original)
{
  auto& components = m_gameState.components();
  if (!components.category.existsFor(original))  return EntityId::Void;

  EntityId newId = EntityId(m_nextEntityId);
  ++m_nextEntityId;

  components.clone(original, newId);

  return newId;
}

void EntityFactory::applyCategoryData(EntityId id, std::string subType, std::string name)
{
  if (id != EntityId::Void)
  {
    json& data = S<IGameRules>().categoryData(name, subType);
    auto& jsonComponents = data["components"];
    m_gameState.components().populate(id, jsonComponents);
  }
  else
  {
    throw std::exception("Attempted to apply category data to Void object!");
  }
}

void EntityFactory::morph(EntityId id, std::string category, std::string material)
{
  auto& components = m_gameState.components();

  if (id == EntityId::Void)
  {
    throw std::exception("Attempted to morph Void object!");
  }

  // First, check if category is being changed.
  std::string oldCategory = components.category.existsFor(id) ? components.category.of(id) : "";

  if (category != oldCategory)
  {
    components.erase(id);
    json& data = S<IGameRules>().categoryData(category);
    auto& jsonComponents = data["components"];
    components.populate(id, data["components"]);
  }

  // Next, check if material is being changed.
  std::string oldMaterial = components.material.existsFor(id) ? components.material.of(id) : "";

  if (material != oldMaterial)
  {
    json& materialData = S<IGameRules>().categoryData(material, "materials");
    components.populate(id, materialData["components"]);
  }

}

void EntityFactory::destroy(EntityId id)
{
  auto& components = m_gameState.components();

  if (id != EntityId::Void)
  {
    // Check for existence.
    // This should work as every entity should have a "category" component.
    if (components.category.existsFor(id))
    {
      components.erase(id);
    }
  }
  else
  {
    throw std::exception("Attempted to destroy Void object!");
  }
}
