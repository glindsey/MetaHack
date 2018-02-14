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
  EntityId nothingness = create({ "Void" });
  Assert("EntityFactory", (nothingness == 0ULL), "Void's ID is " << nothingness << " instead of zero!");

  m_initialized = true;
}

EntityFactory::~EntityFactory()
{
}

EntityId EntityFactory::create(EntitySpecs specs)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = S<IGameRules>().categoryData(specs.category);

  auto& jsonComponents = data["components"];
  m_gameState.components().populate(new_id, jsonComponents);

  auto material = specs.material;
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

  if (!material.empty())
  {
    json& materialData = S<IGameRules>().categoryData("material." + material);
    m_gameState.components().populate(new_id, materialData["components"]);
  }

  if (m_initialized)
  {
    GAME.lua().callEntityFunction("on_create", new_id, {}, true);
  }

  return EntityId(new_id);
}

EntityId EntityFactory::createTileEntity(MapTile* mapTile, EntitySpecs specs)
{
  EntityId new_id = create(specs);

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

void EntityFactory::applyCategoryData(EntityId id, std::string subtypeName)
{
  if (id != EntityId::Void)
  {
    json& subtypeData = S<IGameRules>().categoryData(subtypeName);
    auto& subtypeComponents = subtypeData["components"];
    m_gameState.components().populate(id, subtypeComponents);
  }
  else
  {
    throw std::runtime_error("Attempted to apply category data to Void object!");
  }
}

void EntityFactory::morph(EntityId id, EntitySpecs specs)
{
  auto& components = m_gameState.components();
  CLOG(TRACE, "EntityFactory") << "Changing Entity " << id << " into " << specs.category << "." << specs.material;

  if (id == EntityId::Void)
  {
    throw std::runtime_error("Attempted to morph Void object!");
  }

  // First, check if category is being changed.
  std::string oldCategory = components.category[id];

  if (specs.category != oldCategory)
  {
    json& data = S<IGameRules>().categoryData(specs.category);
    components.populate(id, data["components"]);
  }

  // Next, check if material is being changed.
  std::string oldMaterial = components.material.existsFor(id) ? components.material.of(id) : "";

  if (!specs.material.empty() && (specs.material != oldMaterial))
  {
    json& materialData = S<IGameRules>().categoryData("material." + specs.material);
    components.populate(id, materialData["components"]);
  }

  //CLOG(TRACE, "EntityFactory") << "================================";
  //CLOG(TRACE, "EntityFactory") << "DEBUG: Entity " << id << " components are:";
  //CLOG(TRACE, "EntityFactory") << components.toJSON(id).dump(2);
  //CLOG(TRACE, "EntityFactory") << "================================";

  //std::cerr << "BREAK" << std::endl;
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
    throw std::runtime_error("Attempted to destroy Void object!");
  }
}
