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

void Entity::perform_action_collide_with(EntityId actor)
{
  (void)GAME.lua().callEntityFunction("do_collide_with", m_id, actor, true);
}

void Entity::perform_action_collide_with_wall(Direction d, std::string tile_type)
{
  /// @todo Implement me; right now there's no way to pass one enum and one string to a Lua function.
  return;
}

bool Entity::do_(Actions::Action& action)
{
  return GAME.lua().callEntityFunction("do_" + action.getType(), m_id, {}, true);
}

bool Entity::beObjectOf(Actions::Action& action, EntityId subject)
{
  return GAME.lua().callEntityFunction("on_object_of_" + action.getType(), m_id, subject, true);
}

bool Entity::beObjectOf(Actions::Action& action, EntityId subject, EntityId target)
{
  return GAME.lua().callEntityFunction("on_object_of_" + action.getType(), m_id, { subject, target }, true);
}

bool Entity::beObjectOf(Actions::Action& action, EntityId subject, Direction direction)
{
  return GAME.lua().callEntityFunction("on_object_of_" + action.getType(), m_id, { subject, direction }, true);
}

bool Entity::be_hurt_by(EntityId subject)
{
  return GAME.lua().callEntityFunction("on_object_of_hurt", m_id, subject, true);
}

bool Entity::be_used_to_attack(EntityId subject, EntityId target)
{
  return GAME.lua().callEntityFunction("on_used_to_attack", m_id, { subject, target }, true);
}

bool Entity::can_merge_with(EntityId other) const
{
  // Entities with different types can't merge (obviously).
  if (COMPONENTS.category[other] != COMPONENTS.category[m_id])
  {
    return false;
  }

  // Entities with inventories can never merge.
  if ((COMPONENTS.inventory.valueOrDefault(m_id).maxSize() != 0) ||
    (COMPONENTS.inventory.valueOrDefault(other).maxSize() != 0))
  {
    return false;
  }

  // If the entities have the exact same properties, merge is okay.
  /// @todo Re-implement this to check entity components.
  //if (this and other components match)
  //{
  //  return true;
  //}

  return false;
}

// *** PROTECTED METHODS ******************************************************
