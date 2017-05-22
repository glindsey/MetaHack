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
#include "systems/SystemSpacialRelationships.h"
#include "types/Color.h"
#include "types/Direction.h"
#include "types/Gender.h"
#include "types/IntegerRange.h"
#include "utilities/MathUtils.h"
#include "utilities/Ordinal.h"
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
  initialize();
}

Entity::Entity(Entity const& original, EntityId ref) :
  m_state{ original.m_state },
  m_id{ ref }
{
  COMPONENTS.clone(original.m_id, ref);
  initialize();
}

void Entity::initialize()
{
  /// @todo Nothing to do here? HP/MaxHP are created at component copy time now.
}

Entity::~Entity()
{}

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

bool Entity::actionIsPending() const
{
  return (COMPONENTS.activity.existsFor(m_id) &&
          !COMPONENTS.activity[m_id].pendingActions().empty());
}

bool Entity::actionIsInProgress()
{
  return (COMPONENTS.activity.existsFor(m_id) &&
          COMPONENTS.activity[m_id].busyTicks() > 0);
}

void Entity::clearAllPendingActions()
{
  if (COMPONENTS.activity.existsFor(m_id))
  {
    COMPONENTS.activity[m_id].pendingActions().clear();
  }
}

Gender Entity::getGenderOrYou() const
{
  if (isPlayer())
  {
    return Gender::SecondPerson;
  }
  else
  {
    if (COMPONENTS.physical.valueOrDefault(m_id).quantity() > 1)
    {
      return Gender::Plural;
    }
    else
    {
      return COMPONENTS.gender.valueOrDefault(m_id).gender();
    }
  }
}

/// Get the number of a particular body part the entity typically has.
unsigned int Entity::getBodypartNumber(BodyPart part) const
{
  if (!COMPONENTS.bodyparts.existsFor(m_id)) return 0;
  auto& bodyparts = COMPONENTS.bodyparts[m_id];
  return bodyparts.typicalCount(part);
}

/// Get the appropriate body part name for the DynamicEntity.
std::string Entity::getBodypartName(BodyPart part) const
{
  std::stringstream ss;
  ss << part;

  std::string fancyPartName = "NOUN_" + COMPONENTS.category[m_id] + "_" + ss.str();
  std::string partName = "NOUN_" + ss.str();

  boost::to_upper(fancyPartName);
  boost::to_upper(partName);

  bool fancyNameExists = Service<IStringDictionary>::get().contains(fancyPartName);

  return (fancyNameExists ? tr(fancyPartName) : tr(partName));
}

/// Get the appropriate body part plural for the DynamicEntity.
std::string Entity::getBodypartPlural(BodyPart part) const
{
  std::stringstream ss;
  ss << part;

  std::string fancyPartName = "NOUN_" + COMPONENTS.category[m_id] + "_" + ss.str() + "_PLURAL";
  std::string partName = "NOUN_" + ss.str() + "_PLURAL";

  boost::to_upper(fancyPartName);
  boost::to_upper(partName);

  bool fancyNameExists = Service<IStringDictionary>::get().contains(fancyPartName);

  return (fancyNameExists ? tr(fancyPartName) : tr(partName));
}

bool Entity::isPlayer() const
{
  return (GAME.getPlayer() == m_id);
}

EntityId Entity::getId() const
{
  return m_id;
}

std::string Entity::getDisplayAdjectives() const
{
  std::string adjectives;

  if (COMPONENTS.health.existsFor(m_id) &&
      COMPONENTS.health[m_id].isDead())
  {
    adjectives += tr("ADJECTIVE_DEAD");
  }

  /// @todo Implement more adjectives.

  return adjectives;
}

/// @todo Figure out how to cleanly localize this.
std::string Entity::getDisplayName() const
{
  return getCategoryData().value("name", std::string());
}

/// @todo Figure out how to cleanly localize this.
std::string Entity::getDisplayPlural() const
{
  return getCategoryData().value("plural", std::string());
}

std::string Entity::getSubjectiveString(ArticleChoice articles) const
{
  std::string str;

  if (isPlayer())
  {
    if (COMPONENTS.health.existsFor(m_id) &&
        !COMPONENTS.health[m_id].isDead())
    {
      str = tr("PRONOUN_SUBJECT_YOU");
    }
    else
    {
      str = StringTransforms::make_string_numerical_tokens_only(tr("PRONOUN_POSSESSIVE_YOU"), { tr("NOUN_CORPSE") });
    }
  }
  else
  {
    str = getDescriptiveString(articles);
  }

  return str;
}

std::string Entity::getObjectiveString(ArticleChoice articles) const
{
  std::string str;

  if (isPlayer())
  {
    if (COMPONENTS.health.existsFor(m_id) &&
        !COMPONENTS.health[m_id].isDead())
    {
      str = tr("PRONOUN_OBJECT_YOU");
    }
    else
    {
      str = StringTransforms::make_string_numerical_tokens_only(tr("PRONOUN_POSSESSIVE_YOU"), { tr("NOUN_CORPSE") });
    }
  }
  else
  {
    str = getDescriptiveString(articles);
  }

  return str;
}

std::string Entity::getReflexiveString(EntityId other, ArticleChoice articles) const
{
  if (other == getId())
  {
    return getReflexivePronoun();
  }

  return getDescriptiveString(articles);
}

std::string Entity::getDescriptiveString(ArticleChoice articles,
                                         UsePossessives possessives) const
{
  auto& config = Service<IConfigSettings>::get();

  EntityId location = COMPONENTS.position[m_id].parent();
  unsigned int quantity = COMPONENTS.physical.valueOrDefault(m_id).quantity();

  std::string name;

  bool owned;

  std::string debug_prefix;
  if (config.get("debug-show-thing-ids") == true)
  {
    debug_prefix = "(#" + static_cast<std::string>(getId()) + ") ";
  }

  std::string adjectives;
  std::string noun;
  std::string description;
  std::string suffix;

  owned = COMPONENTS.health.existsFor(location);
  adjectives = getDisplayAdjectives();

  if (quantity == 1)
  {
    noun = getDisplayName();

    if (owned && (possessives == UsePossessives::Yes))
    {
      description = location->getPossessiveString(noun, adjectives);
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        description = tr("ARTICLE_DEFINITE") + " " + adjectives + " " + noun;
      }
      else
      {
        description = getIndefArt(noun) + " " + adjectives + " " + noun;
      }
    }

    if (COMPONENTS.properName.existsFor(m_id))
    {
      auto properName = COMPONENTS.properName[m_id];
      if (!properName.empty())
      {
        suffix = tr("VERB_NAME_PP") + " " + properName;
      }
    }
  }
  else
  {
    noun = std::to_string(COMPONENTS.physical.valueOrDefault(m_id).quantity()) + " " + getDisplayPlural();

    if (owned && (possessives == UsePossessives::Yes))
    {
      description = location->getPossessiveString(noun, adjectives);
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        description = tr("ARTICLE_DEFINITE") + " " + adjectives + " " + noun;
      }
      else
      {
        description = adjectives + " " + noun;
      }
    }
  }

  name = StringTransforms::make_string_numerical_tokens_only(tr("PATTERN_DISPLAY_NAME"), {
    debug_prefix,
    description,
    suffix
  });

  return name;
}

bool Entity::isThirdPerson()
{
  return !((GAME.getPlayer() == m_id) || (COMPONENTS.physical.valueOrDefault(m_id).quantity() > 1));
}

std::string const& Entity::chooseVerb(std::string const& verb12,
                                      std::string const& verb3)
{
  return isThirdPerson() ? verb3 : verb12;
}

std::string const& Entity::getSubjectPronoun() const
{
  return getSubjPro(getGenderOrYou());
}

std::string const& Entity::getObjectPronoun() const
{
  return getObjPro(getGenderOrYou());
}

std::string const& Entity::getReflexivePronoun() const
{
  return getRefPro(getGenderOrYou());
}

std::string const& Entity::getPossessiveAdjective() const
{
  return getPossAdj(getGenderOrYou());
}

std::string const& Entity::getPossessivePronoun() const
{
  return getPossPro(getGenderOrYou());
}

std::string Entity::getPossessiveString(std::string owned, std::string adjectives)
{
  if (GAME.getPlayer() == m_id)
  {
    return StringTransforms::make_string_numerical_tokens_only(tr("PRONOUN_POSSESSIVE_YOU"), { adjectives, owned });
  }
  else
  {
    return StringTransforms::make_string_numerical_tokens_only(tr("PATTERN_POSSESSIVE"), {
      getDescriptiveString(ArticleChoice::Definite, UsePossessives::No),
      adjectives,
      owned
    });
  }
}

/// @todo Make this into an Action.
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

void Entity::destroy()
{
  auto old_location = COMPONENTS.position[m_id].parent();

  if (COMPONENTS.inventory.existsFor(m_id))
  {
    // Spill the contents of this Entity into the Entity's location.
    /// @todo Reimplement me
    //spill();
  }

  if (old_location != EntityId::Mu())
  {
    COMPONENTS.inventory[old_location].remove(m_id);
  }

  COMPONENTS.erase(m_id);

  //notifyObservers(Event::Updated);
}

/// @todo Figure out how to localize this.
std::string Entity::getBodypartDescription(BodyLocation location)
{
  uint32_t total_number = this->getBodypartNumber(location.part);
  std::string part_name = this->getBodypartName(location.part);
  std::string result;

  Assert("Entity", location.number < total_number, "asked for bodypart " << location.number << " of " << total_number);
  switch (total_number)
  {
  case 0: // none of them!?  shouldn't occur!
    result = "non-existent " + part_name;
    CLOG(WARNING, "Entity") << "Request for description of " << result << "!?";
    break;

  case 1: // only one of them
    result = part_name;
    break;

  case 2: // assume a right and left one.
    switch (location.number)
    {
    case 0:
      result = "right " + part_name;
      break;
    case 1:
      result = "left " + part_name;
      break;
    default:
      break;
    }
    break;

  case 3: // assume right, center, and left.
    switch (location.number)
    {
    case 0:
      result = "right " + part_name;
      break;
    case 1:
      result = "center " + part_name;
      break;
    case 2:
      result = "left " + part_name;
      break;
    default:
      break;
    }
    break;

  case 4: // Legs/feet assume front/rear, others assume upper/lower.
    if ((location.part == BodyPart::Leg) || (location.part == BodyPart::Foot))
    {
      switch (location.number)
      {
      case 0:
        result = "front right " + part_name;
        break;
      case 1:
        result = "front left " + part_name;
        break;
      case 2:
        result = "rear right " + part_name;
        break;
      case 3:
        result = "rear left " + part_name;
        break;
      default:
        break;
      }
    }
    else
    {
      switch (location.number)
      {
      case 0:
        result = "upper right " + part_name;
        break;
      case 1:
        result = "upper left " + part_name;
        break;
      case 2:
        result = "lower right " + part_name;
        break;
      case 3:
        result = "lower left " + part_name;
        break;
      default:
        break;
      }
    }
    break;

  case 6: // Legs/feet assume front/middle/rear, others upper/middle/lower.
    if ((location.part == BodyPart::Leg) || (location.part == BodyPart::Foot))
    {
      switch (location.number)
      {
      case 0:
        result = "front right " + part_name;
        break;
      case 1:
        result = "front left " + part_name;
        break;
      case 2:
        result = "middle right " + part_name;
        break;
      case 3:
        result = "middle left " + part_name;
        break;
      case 4:
        result = "rear right " + part_name;
        break;
      case 5:
        result = "rear left " + part_name;
        break;
      default:
        break;
      }
    }
    else
    {
      switch (location.number)
      {
      case 0:
        result = "upper right " + part_name;
        break;
      case 1:
        result = "upper left " + part_name;
        break;
      case 2:
        result = "middle right " + part_name;
        break;
      case 3:
        result = "middle left " + part_name;
        break;
      case 4:
        result = "lower right " + part_name;
        break;
      case 5:
        result = "lower left " + part_name;
        break;
      default:
        break;
      }
    }
    break;

  default:
    break;
  }

  // Anything else and we just return the ordinal name.
  /// @todo Deal with fingers/toes.
  if (result.empty())
  {
    result = Ordinal::get(location.number) + " " + part_name;
  }

  return result;
}

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
      bool action_done = action.process(GAME, {});
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
    if (!isPlayer())
    {
      /// @todo Re-implement me
      (void)call_lua_function("process", {}, true);
    }
  }

  return true;
}
