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
  Subject(),
  m_state{ state },
  m_id{ id },
  m_pending_involuntary_actions{ ActionQueue() },
  m_pending_voluntary_actions{ ActionQueue() }
{
  initialize();
}

Entity::Entity(Entity const& original, EntityId ref) :
  Subject(),
  m_state{ original.m_state },
  m_id{ ref },
  m_pending_involuntary_actions{ ActionQueue() },     // don't copy
  m_pending_voluntary_actions{ ActionQueue() }
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
  CLOG(TRACE, "Entity") << "Entity " <<
    getId() << " (" <<
    COMPONENTS.category[m_id] << "): Queuing Action " <<
    action->getType();

  m_pending_voluntary_actions.push_back(std::move(action));
}

void Entity::queueAction(Actions::Action * p_action)
{
  std::unique_ptr<Actions::Action> action(p_action);
  queueAction(std::move(action));
}

void Entity::queueInvoluntaryAction(std::unique_ptr<Actions::Action> action)
{
  CLOG(TRACE, "Entity") << "Entity " <<
    getId() << " (" <<
    COMPONENTS.category[m_id] << "): Queuing Involuntary Action " <<
    action->getType();

  m_pending_involuntary_actions.push_front(std::move(action));
}

void Entity::queueInvoluntaryAction(Actions::Action * p_action)
{
  std::unique_ptr<Actions::Action> action(p_action);
  queueInvoluntaryAction(std::move(action));
}

bool Entity::anyActionIsPending() const
{
  return voluntaryActionIsPending() || involuntaryActionIsPending();
}

bool Entity::voluntaryActionIsPending() const
{
  return !(m_pending_voluntary_actions.empty());
}

bool Entity::involuntaryActionIsPending() const
{
  return !(m_pending_involuntary_actions.empty());
}

bool Entity::actionIsInProgress()
{
  return (COMPONENTS.busyCounter[m_id] > 0);
}

void Entity::clearAllPendingActions()
{
  clearPendingVoluntaryActions();
  clearPendingInvoluntaryActions();
}

void Entity::clearPendingVoluntaryActions()
{
  m_pending_voluntary_actions.clear();
}

void Entity::clearPendingInvoluntaryActions()
{
  m_pending_involuntary_actions.clear();
}

bool Entity::canReach(EntityId entity)
{
  // Check if it is our location.
  auto our_location = COMPONENTS.position[m_id].parent();
  if (our_location == entity)
  {
    return true;
  }

  // Check if it's at our location.
  auto thing_location = COMPONENTS.position[entity].parent();
  if (our_location == thing_location)
  {
    return true;
  }

  // Check if it's in our inventory.
  if (COMPONENTS.inventory[m_id].contains(entity))
  {
    return true;
  }

  return false;
}

bool Entity::isAdjacentTo(EntityId entity)
{
  // If either doesn't have a position component, bail.
  if (!COMPONENTS.position.existsFor(m_id) ||
      !COMPONENTS.position.existsFor(entity))
  {
    return false;
  }

  auto ourPosition = COMPONENTS.position[m_id];
  auto otherPosition = COMPONENTS.position[entity];

  // If the two are not on the same map, bail.
  if (ourPosition.map() != otherPosition.map())
  {
    return false;
  }

  return adjacent(ourPosition.coords(), otherPosition.coords());
}

void Entity::setGender(Gender gender)
{
  COMPONENTS.gender[m_id].gender() = gender;
}

Gender Entity::getGender() const
{
  return COMPONENTS.gender.valueOrDefault(m_id).gender();
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

bool Entity::canSee(EntityId entity)
{
  // Bail if either doesn't have a Position component.
  if (!COMPONENTS.position.existsFor(m_id) ||
      !COMPONENTS.position.existsFor(entity))
  {
    return false;
  }

  // Bail if the two aren't on the same map.
  auto& thisPosition = COMPONENTS.position[m_id];
  auto& entityPosition = COMPONENTS.position[entity];
  if (thisPosition.map() != entityPosition.map())
  {
    return false;
  }

  return canSee(entityPosition.coords());
}

bool Entity::canSee(IntVec2 coords)
{
  auto& thisPosition = COMPONENTS.position[m_id];
  MapId thisMap = thisPosition.map();
  IntVec2 thisCoords = thisPosition.coords();

  // Make sure we are able to see at all.
  if (!COMPONENTS.senseSight.existsFor(m_id))
  {
    return false;
  }

  // If the coordinates are where we are, then yes, we can indeed see the tile, regardless.
  if ((thisCoords.x == coords.x) && (thisCoords.y == coords.y))
  {
    return true;
  }

  Map& game_map = GAME.maps().get(thisMap);
  auto map_size = game_map.getSize();

  // Check for coords out of bounds. If they're out of bounds, we can't see it.
  if ((coords.x < 0) || (coords.y < 0) || (coords.x >= map_size.x) || (coords.y >= map_size.y))
  {
    return false;
  }

  // Return seen data.
  return COMPONENTS.senseSight[m_id].canSee(coords);
}

void Entity::findSeenTiles()
{
  //sf::Clock elapsed;

  //elapsed.restart();

  // Hang on, can we actually see?
  // If not, bail out.
  if (!COMPONENTS.senseSight.existsFor(m_id))
  {
    return;
  }

  // Are we on a map?  Bail out if we aren't.
  EntityId location = COMPONENTS.position[m_id].parent();
  if (location == EntityId::Mu())
  {
    return;
  }

  // Clear the "tile seen" bitset.
  COMPONENTS.senseSight[m_id].resetSeen();

  /// @todo Handle field-of-view here.
  ///       Field of view for an DynamicEntity can be:
  ///          * NARROW (90 degrees straight ahead)
  ///		   * WIDE (180 degrees in facing direction)
  ///          * FRONTBACK (90 degrees ahead/90 degrees back)
  ///          * FULL (all 360 degrees)
  ComponentPosition const& position = COMPONENTS.position[m_id];

  for (int n = 1; n <= 8; ++n)
  {
    do_recursive_visibility(position, n);
  }
}

MapMemoryChunk const& Entity::getMemoryAt(IntVec2 coords) const
{
  static MapMemoryChunk null_memory_chunk{ "MTUnknown", 0 };

  auto map = COMPONENTS.position[m_id].map();

  if (map == MapFactory::null_map_id)
  {
    return null_memory_chunk;
  }

  if (!COMPONENTS.spacialMemory.existsFor(m_id))
  {
    return null_memory_chunk;
  }

  auto& memory = COMPONENTS.spacialMemory[m_id];

  if (!memory.containsMap(map))
  {
    return null_memory_chunk;
  }

  return memory.ofMap(map).valueOr(coords, null_memory_chunk);
}

bool Entity::moveInto(EntityId newLocation)
{
  // If Entity doesn't have a Position component, bail.
  if (!COMPONENTS.position.existsFor(m_id))
  {
    return false;
  }

  auto& position = COMPONENTS.position[m_id];
  MapId oldMapId = position.map();
  EntityId oldLocation = position.parent();

  if (newLocation == oldLocation)
  {
    // We're already there!
    return true;
  }

  if (newLocation->canContain(m_id))
  {
    if (COMPONENTS.inventory[newLocation].add(m_id) == true)
    {
      // Try to lock our old location.
      if (oldLocation != EntityId::Mu())
      {
        COMPONENTS.inventory[oldLocation].remove(m_id);
      }

      // Set the location to the new location.
      COMPONENTS.position[m_id].set(newLocation);

      MapId newMapId = position.map();
      if (oldMapId != newMapId)
      {
        if (oldMapId != MapFactory::null_map_id)
        {
          /// @todo Save old map memory.
        }

        IntVec2 new_map_size = newMapId->getSize();

        if (COMPONENTS.senseSight.existsFor(m_id))
        {
          COMPONENTS.senseSight[m_id].resizeSeen(new_map_size);
        }

        if (COMPONENTS.spacialMemory.existsFor(m_id))
        {
          auto& spacialMemory = COMPONENTS.spacialMemory[m_id];
          if (!spacialMemory.containsMap(newMapId))
          {
            COMPONENTS.spacialMemory[m_id].ofMap(newMapId).resize(new_map_size);
          }
        }

      }
      this->findSeenTiles();
      //notifyObservers(Event::Updated);
      return true;
    } // end if (add to new inventory was successful)
  } // end if (canContain is true)

  return false;
}

bool Entity::isInsideAnotherEntity() const
{
  auto& parent = COMPONENTS.position[m_id].parent();
  if (parent == EntityId::Mu())
  {
    // Entity is a part of the MapTile such as the floor.
    return false;
  }

  auto& grandparent = COMPONENTS.position[parent].parent();
  if (grandparent == EntityId::Mu())
  {
    // Entity is directly on the floor.
    return false;
  }
  return true;
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

Color Entity::getOpacity() const
{
  return COMPONENTS.appearance.valueOrDefault(m_id).opacity();
}

bool Entity::isOpaque()
{
  return (getOpacity() == Color::White);
}

void Entity::light_up_surroundings()
{
  EntityId location = COMPONENTS.position[m_id].parent();

  // If Entity is not a light source, bail.
  if (!COMPONENTS.lightSource.existsFor(m_id)) return;

  if (COMPONENTS.inventory.existsFor(m_id))
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in an entity's inventory.

    bool opaque = location->isOpaque();
    bool is_entity = COMPONENTS.health.existsFor(m_id);

    //if (!isOpaque() || is wielding(light) || is wearing(light))
    if (!opaque || is_entity)
    {
      auto& inventory = COMPONENTS.inventory[m_id];
      for (auto iter = inventory.begin();
           iter != inventory.end();
           ++iter)
      {
        EntityId entity = iter->second;
        entity->light_up_surroundings();
      }
    }
  }

  // Use visitor pattern.
  if ((location != EntityId::Mu()) &&
      COMPONENTS.lightSource[m_id].lit())
  {
    location->beLitBy(this->getId());
  }
}

void Entity::beLitBy(EntityId light)
{
  // If either this or the light has no Position component, bail.
  if (!COMPONENTS.position.existsFor(m_id) ||
      !COMPONENTS.position.existsFor(light)) return;

  auto& thisPosition = COMPONENTS.position[m_id];
  auto& lightPosition = COMPONENTS.position[light];

  // If this and the light aren't on the same map, bail.
  if (thisPosition.map() != lightPosition.map()) return;

  bool result = call_lua_function("on_lit_by", light, true);
  if (result)
  {
    //notifyObservers(Event::Updated);
  }

  EntityId location = thisPosition.parent();

  if (location == EntityId::Mu())
  {
    thisPosition.map()->addLight(light);
  }
  else
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in the player's inventory.

    bool opaque = this->isOpaque();
    bool is_entity = COMPONENTS.health.existsFor(m_id);

    //if (!isOpaque() || is wielding(light) || is wearing(light))
    if (!opaque || is_entity)
    {
      location->beLitBy(light);
    }
  }
}

/// @todo Make this into an Action.
void Entity::spill()
{
  ComponentInventory& inventory = COMPONENTS.inventory[m_id];
  std::string message;
  bool success = false;

  // Step through all contents of this Entity.
  for (auto iter = inventory.begin();
       iter != inventory.end();
       ++iter)
  {
    EntityId entity = iter->second;
    auto parent = COMPONENTS.position[m_id].parent();
    if (parent != EntityId::Mu())
    {
      if (parent->canContain(entity))
      {
        // Try to move this into the Entity's location.
        success = entity->moveInto(parent);
        if (success)
        {
          auto container_string = this->getDescriptiveString();
          auto thing_string = entity->getDescriptiveString();
          message = thing_string + this->chooseVerb(" tumble", " tumbles") + " out of " + container_string + ".";
          Service<IMessageLog>::get().add(message);
        }
        else
        {
          // We couldn't move it, so just destroy it.
          auto container_string = this->getDescriptiveString();
          auto thing_string = entity->getDescriptiveString();
          message = thing_string + this->chooseVerb(" vanish", " vanishes") + " in a puff of logic.";
          Service<IMessageLog>::get().add(message);
          entity->destroy();
        }
        //notifyObservers(Event::Updated);

      } // end if (canContain)
    } // end if (container location is not Mu)
    else
    {
      // Container's location is Mu, so just destroy it without a message.
      entity->destroy();
    }
  } // end for (contents of Entity)
}

void Entity::destroy()
{
  auto old_location = COMPONENTS.position[m_id].parent();

  if (COMPONENTS.inventory.existsFor(m_id))
  {
    // Spill the contents of this Entity into the Entity's location.
    spill();
  }

  if (old_location != EntityId::Mu())
  {
    COMPONENTS.inventory[old_location].remove(m_id);
  }

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

bool Entity::process_involuntary_actions()
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
    /* bool dead = */ entity->process_involuntary_actions();
  }

  // Process self last.
  return _process_own_involuntary_actions();
}

bool Entity::process_voluntary_actions()
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
    /* bool dead = */ entity->process_voluntary_actions();
  }

  // Process self last.
  return _process_own_voluntary_actions();
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

bool Entity::canContain(EntityId entity)
{
  size_t maxSize = COMPONENTS.inventory[m_id].maxSize();
  if (maxSize == 0)
  {
    return false;
  }
  else if (COMPONENTS.inventory[m_id].count() >= maxSize)
  {
    return false;
  }
  else
  {
    return call_lua_function("can_contain", entity, true);
  }
}

json Entity::call_lua_function(std::string function_name,
                               json const& args,
                               json const& default_result)
{
  return the_lua_instance.call_thing_function(function_name, getId(), args, default_result);
}

json Entity::call_lua_function(std::string function_name,
                               json const& args,
                               json const& default_result) const
{
  return the_lua_instance.call_thing_function(function_name, getId(), args, default_result);
}

std::unordered_set<EventID> Entity::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  /// @todo Add our own events here
  return events;
}

// *** PROTECTED METHODS ******************************************************

bool Entity::_process_own_involuntary_actions()
{
  bool entity_updated = false;

  // Is this an entity that is now dead?
  if (COMPONENTS.health.existsFor(m_id) &&
      COMPONENTS.health[m_id].hasHpBelowZero())
  {
    // Did the entity JUST die?
    if (!COMPONENTS.health[m_id].isDead())
    {
      // Perform the "die" action.
      // (This sets the "dead" property and clears out any pending actions.)
      std::unique_ptr<Actions::Action> dieAction(NEW Actions::ActionDie(getId()));
      this->queueInvoluntaryAction(std::move(dieAction));
    }
  }

  // While actions are pending...
  while (!m_pending_involuntary_actions.empty())
  {
    // Process the front action.
    // @todo Find a way to update the entity_updated variable.
    std::unique_ptr<Actions::Action>& action = m_pending_involuntary_actions.front();
    bool action_done = action->process({});
    if (action_done)
    {
      CLOG(TRACE, "Entity") << "Entity " <<
        getId() << " (" <<
        COMPONENTS.category[m_id] << "): Involuntary Action " <<
        action->getType() << " is done, popping";

      m_pending_involuntary_actions.pop_front();
    }
  } // end while (actions pending)

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

  return true;
}

bool Entity::_process_own_voluntary_actions()
{
  auto& busyCounter = COMPONENTS.busyCounter[m_id];

  // Is this an entity that is now dead?
  if (COMPONENTS.health.existsFor(m_id) &&
      COMPONENTS.health[m_id].hasHpBelowZero())
  {
    // Did the entity JUST die?
    if (!COMPONENTS.health[m_id].isDead())
    {
      // Perform the "die" action.
      // (This sets the "dead" property and clears out any pending actions.)
      std::unique_ptr<Actions::Action> dieAction(NEW Actions::ActionDie(getId()));
      this->queueInvoluntaryAction(std::move(dieAction));
    }
  }

  // If this entity is busy...
  if (busyCounter > 0)
  {
    // Decrement busy counter.
    --busyCounter;
  }
  // Otherwise if there are pending actions...
  else if (!m_pending_voluntary_actions.empty())
  {
    bool entity_updated = false;

    // While actions are pending and we're not busy...
    while (!m_pending_voluntary_actions.empty() && busyCounter == 0)
    {
      // Process the front action.
      // @todo Find a way to update the entity_updated variable.
      std::unique_ptr<Actions::Action>& action = m_pending_voluntary_actions.front();
      bool action_done = action->process({});
      if (action_done)
      {
        CLOG(TRACE, "Entity") << "Entity " <<
          getId() << " (" <<
          COMPONENTS.category[m_id] << "): Action " <<
          action->getType() << " is done, popping";

        m_pending_voluntary_actions.pop_front();
      }
    } // end while (actions pending and not busy)

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

void Entity::do_recursive_visibility(ComponentPosition const& thisPosition,
                                     int octant,
                                     int depth,
                                     float slope_A,
                                     float slope_B)
{
  Assert("Entity", octant >= 1 && octant <= 8, "Octant" << octant << "passed in is not between 1 and 8 inclusively");
  IntVec2 newCoords;
  //int x = 0;
  //int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (isInsideAnotherEntity())
  {
    return;
  }

  IntVec2 thisCoords = thisPosition.coords();
  MapId thisMap = thisPosition.map();

  static const int mv = 128;
  static constexpr int mw = (mv * mv);

  std::function< bool(RealVec2, RealVec2, float) > loop_condition;
  Direction dir;
  std::function< float(RealVec2, RealVec2) > recurse_slope;
  std::function< float(RealVec2, RealVec2) > loop_slope;

  switch (octant)
  {
  case 1:
    newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) - (slope_A * static_cast<float>(depth))));
    newCoords.y = thisCoords.y - depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
    dir = Direction::West;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
    break;

  case 2:
    newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) + (slope_A * static_cast<float>(depth))));
    newCoords.y = thisCoords.y - depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
    dir = Direction::East;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Northeast.half(), b); };
    break;

  case 3:
    newCoords.x = thisCoords.x + depth;
    newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) - (slope_A * static_cast<float>(depth))));
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
    dir = Direction::North;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
    break;

  case 4:
    newCoords.x = thisCoords.x + depth;
    newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) + (slope_A * static_cast<float>(depth))));
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
    dir = Direction::South;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
    break;

  case 5:
    newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) + (slope_A * static_cast<float>(depth))));
    newCoords.y = thisCoords.y + depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
    dir = Direction::East;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
    break;

  case 6:
    newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) - (slope_A * static_cast<float>(depth))));
    newCoords.y = thisCoords.y + depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
    dir = Direction::West;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Southwest.half(), b); };
    break;

  case 7:
    newCoords.x = thisCoords.x - depth;
    newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) + (slope_A * static_cast<float>(depth))));
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
    dir = Direction::South;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
    break;

  case 8:
    newCoords.x = thisCoords.x - depth;
    newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) - (slope_A * static_cast<float>(depth))));

    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
    dir = Direction::North;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
    break;

  default:
    break;
  }

  while (loop_condition(to_v2f(newCoords), to_v2f(thisCoords), slope_B))
  {
    if (calc_vis_distance(newCoords, thisCoords) <= mw)
    {
      if (thisMap->tileIsOpaque(newCoords))
      {
        if (!thisMap->tileIsOpaque(newCoords + (IntVec2)dir))
        {
          do_recursive_visibility(thisPosition,
                                  octant, depth + 1,
                                  slope_A, recurse_slope(to_v2f(newCoords), to_v2f(thisCoords)));
        }
      }
      else
      {
        if (thisMap->tileIsOpaque(newCoords + (IntVec2)dir))
        {
          slope_A = loop_slope(to_v2f(newCoords), to_v2f(thisCoords));
        }
      }

      if (COMPONENTS.senseSight.existsFor(m_id))
      {
        COMPONENTS.senseSight[m_id].setSeen(newCoords);
      }

      if (COMPONENTS.spacialMemory.existsFor(m_id))
      {
        MapMemoryChunk new_memory{ thisMap->getTile(newCoords).getTileType(),
          GAME.getGameClock() };

        COMPONENTS.spacialMemory[m_id].ofMap(thisMap)[newCoords] = new_memory;
      }
    }
    newCoords -= (IntVec2)dir;
  }
  newCoords += (IntVec2)dir;

  if ((depth < mv) && (!thisMap->getTile(newCoords).isOpaque()))
  {
    do_recursive_visibility(thisPosition,
                            octant, depth + 1,
                            slope_A, slope_B);
  }
}