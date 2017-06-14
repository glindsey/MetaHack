#include "stdafx.h"

#include "entity/Entity.h"

#include "actions/Action.h"
#include "actions/ActionDie.h"
#include "AssertHelper.h"
#include "components/ComponentInventory.h"
#include "components/ComponentManager.h"
#include "components/ComponentMap.h"
#include "components/ComponentPhysical.h"
#include "components/ComponentPosition.h"
#include "entity/EntityFactory.h"
#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "maptile/MapTile.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "services/IMessageLog.h"
#include "services/IStrings.h"
#include "systems/Manager.h"
#include "systems/SystemGeometry.h"
#include "types/Color.h"
#include "types/Direction.h"
#include "types/Gender.h"
#include "types/IntegerRange.h"
#include "utilities/MathUtils.h"
#include "utilities/Ordinal.h"
#include "utilities/Shortcuts.h"
#include "utilities/StringTransforms.h"


// Static member initialization.
Color const Entity::wall_outline_color_{ 255, 255, 255, 64 };

Entity::Entity(GameState& state, std::string category, EntityId id) :
  m_state{ state },
  m_id{ id }
{
}

Entity::Entity(Entity const& original, EntityId ref) :
  m_state{ original.m_state },
  m_id{ ref }
{
  COMPONENTS.clone(original.m_id, ref);
}

Entity::~Entity()
{}

std::string Entity::getDisplayName() const
{
  return COMPONENTS.category[m_id];
}

EntityId Entity::getId() const
{
  return m_id;
}