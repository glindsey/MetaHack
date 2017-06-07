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
#include "entity/EntityPool.h"
#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "maptile/MapTile.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
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

json const& Entity::getCategoryData() const
{
  return Service<IGameRules>::get().category(COMPONENTS.category[m_id]);
}

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

void Entity::queueAction(std::unique_ptr<Actions::Action> action)
{
  if (COMPONENTS.activity.existsFor(m_id))
  {
    CLOG(TRACE, "Entity") << "Entity " <<
      getId() << " (" <<
      COMPONENTS.category[m_id] << "): Queuing Action " <<
      action->getType();

    COMPONENTS.activity[m_id].pendingActions().push(std::move(action));
  }
  else
  {
    CLOG(WARNING, "Entity") << "Entity " <<
      getId() << " (" <<
      COMPONENTS.category[m_id] << "): Tried to queue Action " <<
      action->getType() << ", but entity does not have Activity component";
  }
}

void Entity::queueAction(Actions::Action * p_action)
{
  std::unique_ptr<Actions::Action> action(p_action);
  queueAction(std::move(action));
}

EntityId Entity::getId() const
{
  return m_id;
}

// @todo Make this into an Action.
//void Entity::spill()
//{
//  ComponentInventory& inventory = COMPONENTS.inventory[m_id];
//  std::string message;
//  bool success = false;
//
//  // Step through all contents of this Entity.
//  for (auto iter = inventory.begin();
//       iter != inventory.end();
//       ++iter)
//  {
//    EntityId entity = iter->second;
//    auto parent = COMPONENTS.position[m_id].parent();
//    if (parent != EntityId::Mu())
//    {
//      if (parent->canContain(entity))
//      {
//        // Try to move this into the Entity's location.
//        success = entity->moveInto(parent);
//        if (success)
//        {
//          auto container_string = this->getDescriptiveString();
//          auto thing_string = entity->getDescriptiveString();
//          message = thing_string + this->chooseVerb(" tumble", " tumbles") + " out of " + container_string + ".";
//          Service<IMessageLog>::get().add(message);
//        }
//        else
//        {
//          // We couldn't move it, so just destroy it.
//          auto container_string = this->getDescriptiveString();
//          auto thing_string = entity->getDescriptiveString();
//          message = thing_string + this->chooseVerb(" vanish", " vanishes") + " in a puff of logic.";
//          Service<IMessageLog>::get().add(message);
//          entity->destroy();
//        }
//        //notifyObservers(Event::Updated);
//
//      } // end if (canContain)
//    } // end if (container location is not Mu)
//    else
//    {
//      // Container's location is Mu, so just destroy it without a message.
//      entity->destroy();
//    }
//  } // end for (contents of Entity)
//}

BodyPart Entity::is_equippable_on() const
{
  return BodyPart::MemberCount;
}

bool Entity::processActions()
{
  // Get a copy of the Entity's inventory.
  // This is because entities can be deleted/removed from the inventory
  // over the course of processing them, and this could invalidate the
  // iterator.
  ComponentInventory temp_inventory{ COMPONENTS.inventory[m_id] };

  // Process inventory.
  for (auto iter = temp_inventory.begin();
       iter != temp_inventory.end();
       ++iter)
  {
    EntityId entity = iter->second;
    /* bool dead = */ entity->processActions();
  }

  // Process self last.
  return processOwnActions_();
}

bool Entity::perform_action_die()
{
  return call_lua_function("do_die", {}, true);
}

void Entity::perform_action_collide_with(EntityId actor)
{
  (void)call_lua_function("do_collide_with", actor, true);
}

void Entity::perform_action_collide_with_wall(Direction d, std::string tile_type)
{
  /// @todo Implement me; right now there's no way to pass one enum and one string to a Lua function.
  return;
}

bool Entity::do_(Actions::Action& action)
{
  return call_lua_function("do_" + action.getType(), {}, true);
}

bool Entity::beObjectOf(Actions::Action& action, EntityId subject)
{
  return call_lua_function("on_object_of_" + action.getType(), subject, true);
}

bool Entity::beObjectOf(Actions::Action& action, EntityId subject, EntityId target)
{
  return call_lua_function("on_object_of_" + action.getType(), { subject, target }, true);
}

bool Entity::beObjectOf(Actions::Action& action, EntityId subject, Direction direction)
{
  return call_lua_function("on_object_of_" + action.getType(), { subject, direction }, true);
}

bool Entity::be_hurt_by(EntityId subject)
{
  return call_lua_function("on_object_of_hurt", subject, true);
}

bool Entity::be_used_to_attack(EntityId subject, EntityId target)
{
  return call_lua_function("on_used_to_attack", { subject, target }, true);
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

json Entity::call_lua_function(std::string function_name,
                               json const& args,
                               json const& default_result)
{
  return GAME.lua().call_thing_function(function_name, getId(), args, default_result);
}

json Entity::call_lua_function(std::string function_name,
                               json const& args,
                               json const& default_result) const
{
  return GAME.lua().call_thing_function(function_name, getId(), args, default_result);
}

// *** PROTECTED METHODS ******************************************************

bool Entity::processOwnActions_()
{
  if (!COMPONENTS.activity.existsFor(m_id)) return false;

  auto& activity = COMPONENTS.activity[m_id];

  /// @todo This all gets moved into the "GrimReaper" system.
  //// Is this an entity that is now dead?
  //if (COMPONENTS.health.existsFor(m_id) &&
  //    COMPONENTS.health[m_id].hasHpBelowZero())
  //{
  //  // Did the entity JUST die?
  //  if (!COMPONENTS.health[m_id].isDead())
  //  {
  //    // Perform the "die" action.
  //    // (This sets the "dead" property and clears out any pending actions.)
  //    std::unique_ptr<Actions::Action> dieAction(NEW Actions::ActionDie(getId()));
  //    this->queueInvoluntaryAction(std::move(dieAction));
  //  }
  //}

  // If there are pending actions...
  if (!activity.pendingActions().empty())
  {
    bool entity_updated = false;

    do 
    {
      /// Process the front action until we are marked as busy, 
      /// or the action is done.
      /// @todo Find a way to update the entity_updated variable.
      Actions::Action& action = activity.pendingActions().front();
      bool action_done = action.process(GAME, SYSTEMS);
      if (action_done)
      {
        CLOG(TRACE, "Entity") << "Entity " <<
          getId() << " (" <<
          COMPONENTS.category[m_id] << "): Action " <<
          action.getType() << " is done, popping";

        activity.pendingActions().pop();
      }
    } while (!activity.pendingActions().empty() && activity.busyTicks() == 0); // loop while (actions pending and not busy)

    if (entity_updated)
    {
      /// @todo This needs to be changed so it only is called if the Action
      ///       materially affected the entity in some way. Two ways to do this
      ///       that I can see:
      ///         1) The Action calls notifyObservers. Requires the Action
      ///            to have access to that method; right now it doesn't.
      ///         2) The Action returns some sort of indication that the Entity
      ///            was modified as a result. This could be done by changing
      ///            the return type from a bool to a struct of some sort.
      //notifyObservers(Event::Updated);
    }

  } // end if (actions pending)
  // Otherwise if there are no pending actions...
  else
  {
    // If entity is not the player, call the Lua process function on this 
    // Entity, which runs the AI and may queue new actions.
    if (COMPONENTS.globals.player() != m_id)
    {
      /// @todo Re-implement me
      (void)call_lua_function("process", {}, true);
    }
  }

  return true;
}
