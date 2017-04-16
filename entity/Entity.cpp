#include "stdafx.h"

#include "entity/Entity.h"

#include "actions/Action.h"
#include "actions/ActionDie.h"
#include "AssertHelper.h"
#include "components/ComponentManager.h"
#include "components/ComponentMap.h"
#include "components/ComponentPosition.h"
#include "entity/EntityPool.h"
#include "game/App.h"
#include "game/GameState.h"
#include "inventory/Inventory.h"
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
  return Service<IGameRules>::get().category(m_category);
}

Entity::Entity(GameState& state, std::string category, EntityId id)
  :
  m_state{ state },
  m_category{ category },
  m_properties{ id },
  m_id{ id },
  m_map_tile{ nullptr },
  m_inventory{ Inventory() },
  m_gender{ Gender::None },
  m_map_memory{ MapMemory() },
  m_tiles_currently_seen{ TilesSeen() },
  m_pending_involuntary_actions{ ActionQueue() },
  m_pending_voluntary_actions{ ActionQueue() },
  m_wielded_items{ BodyLocationMap() },
  m_equipped_items{ BodyLocationMap() }
{
  /// @todo This should really be done by the EntityPool, based on the
  ///       components present in the category XML file.
  COMPONENTS.position.add(id);
  initialize();
}

Entity::Entity(GameState& state, MapTile* map_tile, std::string category, EntityId id)
  :
  m_state{ state },
  m_category{ category },
  m_properties{ id },
  m_id{ id },
  m_map_tile{ map_tile },
  m_inventory{ Inventory() },
  m_gender{ Gender::None },
  m_map_memory{ MapMemory() },
  m_tiles_currently_seen{ TilesSeen() },
  m_pending_involuntary_actions{ ActionQueue() },
  m_pending_voluntary_actions{ ActionQueue() },
  m_wielded_items{ BodyLocationMap() },
  m_equipped_items{ BodyLocationMap() }
{
  IntVec2 position = map_tile->getCoords();
  COMPONENTS.position[id] = position;
  initialize();
}

Entity::Entity(Entity const& original, EntityId ref)
  :
  m_state{ original.m_state },
  m_category{ original.m_category },
  m_properties{ original.m_properties },
  m_id{ ref },
  m_map_tile{ original.m_map_tile },
  m_inventory{ Inventory() },             // don't copy
  m_gender{ original.m_gender },
  m_map_memory{ original.m_map_memory },
  m_tiles_currently_seen{ TilesSeen() },  // don't copy
  m_pending_involuntary_actions{ ActionQueue() },     // don't copy
  m_pending_voluntary_actions{ ActionQueue() },     // don't copy
  m_wielded_items{ BodyLocationMap() },       // don't copy
  m_equipped_items{ BodyLocationMap() }        // don't copy
{
  /// @todo A ComponentManager method should do this copy.
  COMPONENTS.position[ref] = COMPONENTS.position[original.m_id];
  initialize();
}

void Entity::initialize()
{
  /// Get our maximum HP. (The method will automatically pick it from a range.)
  /// @todo Create an "Integer" type that contains a from_json that handles
  ///       ranges.
  //auto max_hp = m_type_data.value("maxhp", 1);
  int max_hp = 10;

  /// Set our starting HP to that value.
  setBaseProperty("hp", max_hp);

  //notifyObservers(Event::Updated);
}

Entity::~Entity()
{
}

void Entity::queueAction(std::unique_ptr<Actions::Action> action)
{
  CLOG(TRACE, "Entity") << "Entity " <<
    getId() << " (" <<
    getCategory() << "): Queuing Action " <<
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
    getCategory() << "): Queuing Involuntary Action " <<
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
  return (getBaseProperty("counter-busy", 0).get<int>() > 0);
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

EntityId Entity::getWieldingIn(BodyLocation& location)
{
  if (m_wielded_items.count(location) == 0)
  {
    return EntityId::Mu();
  }
  else
  {
    return m_wielded_items[location];
  }
}

bool Entity::isWielding(EntityId entity)
{
  BodyLocation dummy;
  return isWielding(entity, dummy);
}

bool Entity::isWielding(EntityId entity, BodyLocation& location)
{
  if (entity == EntityId::Mu())
  {
    return false;
  }
  auto found_item =
    std::find_if(m_wielded_items.cbegin(),
                 m_wielded_items.cend(),
                 [&](BodyLocationPair const& p)
  { return p.second == entity; });

  if (found_item == m_wielded_items.cend())
  {
    return false;
  }
  else
  {
    location = found_item->first;
    return true;
  }
}

bool Entity::isWearing(EntityId entity)
{
  BodyLocation dummy;
  return isWearing(entity, dummy);
}

bool Entity::isWearing(EntityId entity, BodyLocation& location)
{
  if (entity == EntityId::Mu())
  {
    return false;
  }
  auto found_item =
    std::find_if(m_equipped_items.cbegin(),
                 m_equipped_items.cend(),
                 [&](BodyLocationPair const& p)
  { return p.second == entity; });

  if (found_item == m_equipped_items.cend())
  {
    return false;
  }
  else
  {
    location = found_item->first;
    return true;
  }
}

bool Entity::canReach(EntityId entity)
{
  // Check if it is our location.
  auto our_location = getLocation();
  if (our_location == entity)
  {
    return true;
  }

  // Check if it's at our location.
  auto thing_location = entity->getLocation();
  if (our_location == thing_location)
  {
    return true;
  }

  // Check if it's in our inventory.
  if (this->getInventory().contains(entity))
  {
    return true;
  }

  return false;
}

bool Entity::isAdjacentTo(EntityId entity)
{
  // Get the coordinates we are at.
  MapTile* our_maptile = getMapTile();
  MapTile* thing_maptile = entity->getMapTile();
  if ((our_maptile == nullptr) || (thing_maptile == nullptr))
  {
    return false;
  }

  IntVec2 const& our_coords = our_maptile->getCoords();
  IntVec2 const& thing_coords = thing_maptile->getCoords();

  return adjacent(our_coords, thing_coords);
}

void Entity::setWielded(EntityId entity, BodyLocation location)
{
  if (entity == EntityId::Mu())
  {
    m_wielded_items.erase(location);
  }
  else
  {
    m_wielded_items[location] = entity;
  }
}

void Entity::setWorn(EntityId entity, BodyLocation location)
{
  if (entity == EntityId::Mu())
  {
    m_equipped_items.erase(location);
  }
  else
  {
    m_equipped_items[location] = entity;
  }
}

bool Entity::canCurrentlySee()
{
  return getModifiedProperty("can-see", false);
}

bool Entity::canCurrentlyMove()
{
  return getModifiedProperty("can-move", false);
}

void Entity::setGender(Gender gender)
{
  m_gender = gender;
}

Gender Entity::getGender() const
{
  return m_gender;
}

Gender Entity::getGenderOrYou() const
{
  if (isPlayer())
  {
    return Gender::SecondPerson;
  }
  else
  {
    if (getQuantity() > 1)
    {
      return Gender::Plural;
    }
    else
    {
      return m_gender;
    }
  }
}

/// Get the number of a particular body part the DynamicEntity has.
unsigned int Entity::getBodypartNumber(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return getIntrinsic("bodypart-body-count", 0);
    case BodyPart::Skin:
      return getIntrinsic("bodypart-skin-count", 0);
    case BodyPart::Head:
      return getIntrinsic("bodypart-head-count", 0);
    case BodyPart::Ear:
      return getIntrinsic("bodypart-ear-count", 0);
    case BodyPart::Eye:
      return getIntrinsic("bodypart-eye-count", 0);
    case BodyPart::Nose:
      return getIntrinsic("bodypart-nose-count", 0);
    case BodyPart::Mouth:
      return getIntrinsic("bodypart-mouth-count", 0);
    case BodyPart::Neck:
      return getIntrinsic("bodypart-neck-count", 0);
    case BodyPart::Chest:
      return getIntrinsic("bodypart-chest-count", 0);
    case BodyPart::Arm:
      return getIntrinsic("bodypart-arm-count", 0);
    case BodyPart::Hand:
      return getIntrinsic("bodypart-hand-count", 0);
    case BodyPart::Leg:
      return getIntrinsic("bodypart-leg-count", 0);
    case BodyPart::Foot:
      return getIntrinsic("bodypart-foot-count", 0);
    case BodyPart::Wing:
      return getIntrinsic("bodypart-wing-count", 0);
    case BodyPart::Tail:
      return getIntrinsic("bodypart-tail-count", 0);
    default:
      return 0;
  }
}

/// Get the appropriate body part name for the DynamicEntity.
/// @todo Figure out how to cleanly localize this.
std::string Entity::getBodypartName(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return getIntrinsic("bodypart-body-name", "[body]");
    case BodyPart::Skin:
      return getIntrinsic("bodypart-skin-name", "[skin]");
    case BodyPart::Head:
      return getIntrinsic("bodypart-head-name", "[head]");
    case BodyPart::Ear:
      return getIntrinsic("bodypart-ear-name", "[ear]");
    case BodyPart::Eye:
      return getIntrinsic("bodypart-eye-name", "[eye]");
    case BodyPart::Nose:
      return getIntrinsic("bodypart-nose-name", "[nose]");
    case BodyPart::Mouth:
      return getIntrinsic("bodypart-mouth-name", "[mouth]");
    case BodyPart::Neck:
      return getIntrinsic("bodypart-neck-name", "[neck]");
    case BodyPart::Chest:
      return getIntrinsic("bodypart-chest-name", "[chest]");
    case BodyPart::Arm:
      return getIntrinsic("bodypart-arm-name", "[arm]");
    case BodyPart::Hand:
      return getIntrinsic("bodypart-hand-name", "[hand]");
    case BodyPart::Leg:
      return getIntrinsic("bodypart-leg-name", "[leg]");
    case BodyPart::Foot:
      return getIntrinsic("bodypart-foot-name", "[foot]");
    case BodyPart::Wing:
      return getIntrinsic("bodypart-wing-name", "[wing]");
    case BodyPart::Tail:
      return getIntrinsic("bodypart-tail-name", "[tail]");
    default:
      return "squeedlyspooch (unknown BodyPart)";
  }
}

/// Get the appropriate body part plural for the DynamicEntity.
/// @todo Figure out how to cleanly localize this.
std::string Entity::getBodypartPlural(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return getIntrinsic("bodypart-body-plural", "[bodies]");
    case BodyPart::Skin:
      return getIntrinsic("bodypart-skin-plural", "[skins]");
    case BodyPart::Head:
      return getIntrinsic("bodypart-head-plural", "[hands]");
    case BodyPart::Ear:
      return getIntrinsic("bodypart-ear-plural", "[ears]");
    case BodyPart::Eye:
      return getIntrinsic("bodypart-eye-plural", "[eyes]");
    case BodyPart::Nose:
      return getIntrinsic("bodypart-nose-plural", "[noses]");
    case BodyPart::Mouth:
      return getIntrinsic("bodypart-mouth-plural", "[mouths]");
    case BodyPart::Neck:
      return getIntrinsic("bodypart-neck-plural", "[necks]");
    case BodyPart::Chest:
      return getIntrinsic("bodypart-chest-plural", "[chests]");
    case BodyPart::Arm:
      return getIntrinsic("bodypart-arm-plural", "[arms]");
    case BodyPart::Hand:
      return getIntrinsic("bodypart-hand-plural", "[hands]");
    case BodyPart::Leg:
      return getIntrinsic("bodypart-leg-plural", "[legs]");
    case BodyPart::Foot:
      return getIntrinsic("bodypart-foot-plural", "[feet]");
    case BodyPart::Wing:
      return getIntrinsic("bodypart-wing-plural", "[wings]");
    case BodyPart::Tail:
      return getIntrinsic("bodypart-tail-plural", "[tails]");
    default:
      return "squeedlyspooches (unknown BodyParts)";
  }
}

bool Entity::isPlayer() const
{
  return (GAME.getPlayer() == m_id);
}

std::string const& Entity::getCategory() const
{
  return m_category;
}

std::string Entity::getParentCategory() const
{
  return getCategoryData().value("parent", "");
}

bool Entity::isSubtypeOf(std::string that_type) const
{
  std::string this_type = getCategory();
  return GAME.entities().firstIsSubtypeOfSecond(this_type, that_type);
}

json Entity::getIntrinsic(std::string key, json default_value) const
{
  return getCategoryData().value(key, default_value);
}

json Entity::getBaseProperty(std::string key, json default_value) const
{
  if (m_properties.contains(key))
  {
    return m_properties.get(key);
  }
  else
  {
    auto value = getCategoryData().value(key, default_value);
    m_properties.set(key, value);
    return value;
  }
}

bool Entity::setBaseProperty(std::string key, json value)
{
  bool existed = m_properties.contains(key);
  m_properties.set(key, value);

  return existed;
}

void Entity::addToBaseProperty(std::string key, json add_value)
{
  json existing_value = m_properties.get(key);
  if (existing_value.is_number_float())
  {
    m_properties.set(key, existing_value.get<double>() + add_value.get<double>());
  }
  else if (existing_value.is_number_integer())
  {
    if (existing_value.is_number_unsigned())
    {
      m_properties.set(key, existing_value.get<unsigned int>() + add_value.get<unsigned int>());
    }
    else
    {
      m_properties.set(key, existing_value.get<int>() + add_value.get<int>());
    }
  }
  else if (existing_value.is_string())
  {
    m_properties.set(key, existing_value.get<std::string>() + add_value.get<std::string>());
  }
  else
  {
    Assert("Entity", false, "Attempted to add to non-addable property \"" << key << "\"");
  }
}

json Entity::getModifiedProperty(std::string key, json default_value) const
{
  if (!m_properties.contains(key))
  {
    json value = getCategoryData().value(key, default_value);
    m_properties.set(key, value);
  }

  return m_properties.get_modified(key);
}

bool Entity::addModifier(std::string key, EntityId id, PropertyModifierInfo const& info)
{
  return m_properties.addModifier(key, id, info);
}

size_t Entity::removeModifier(std::string key, EntityId id)
{
  return m_properties.removeModifier(key, id);
}

unsigned int Entity::getQuantity() const
{
  return getBaseProperty("quantity", 1);
}

void Entity::setQuantity(unsigned int quantity)
{
  setBaseProperty("quantity", quantity);
}

EntityId Entity::getId() const
{
  return m_id;
}

EntityId Entity::getLocation() const
{
  return COMPONENTS.position[m_id].parent();
}

bool Entity::canSee(EntityId entity)
{
  // Make sure we are able to see at all.
  if (!canCurrentlySee())
  {
    return false;
  }

  // Are we on a map?  Bail out if we aren't.
  MapId entity_map_id = this->getMapId();
  MapId thing_map_id = entity->getMapId();

  if ((entity_map_id == MapFactory::null_map_id) ||
    (thing_map_id == MapFactory::null_map_id) ||
      (entity_map_id != thing_map_id))
  {
    return false;
  }

  auto thing_location = entity->getMapTile();
  if (thing_location == nullptr)
  {
    return false;
  }

  IntVec2 thing_coords = thing_location->getCoords();

  return canSee(thing_coords);
}

bool Entity::canSee(IntVec2 coords)
{
  // Make sure we are able to see at all.
  if (!canCurrentlySee())
  {
    return false;
  }

  MapId map_id = getMapId();

  // Are we on a map?  Bail out if we aren't.
  if (map_id == MapFactory::null_map_id)
  {
    return false;
  }

  // If we aren't on a valid maptile, we can't see anything.
  auto tile = getMapTile();
  if (tile == nullptr)
  {
    return false;
  }

  // If the coordinates are where we are, then yes, we can indeed see the tile, regardless.
  IntVec2 tile_coords = tile->getCoords();

  if ((tile_coords.x == coords.x) && (tile_coords.y == coords.y))
  {
    return true;
  }

  Map& game_map = GAME.maps().get(map_id);
  auto map_size = game_map.getSize();

  // Check for coords out of bounds. If they're out of bounds, we can't see it.
  if ((coords.x < 0) || (coords.y < 0) || (coords.x >= map_size.x) || (coords.y >= map_size.y))
  {
    return false;
  }

  // Return seen data.
  return m_tiles_currently_seen[game_map.getIndex(coords)];
}

void Entity::findSeenTiles()
{
  //sf::Clock elapsed;

  //elapsed.restart();

  // Are we on a map?  Bail out if we aren't.
  EntityId location = getLocation();
  if (location == EntityId::Mu())
  {
    return;
  }

  // Clear the "tile seen" bitset.
  m_tiles_currently_seen.reset();

  // Hang on, can we actually see?
  // If not, bail out.
  if (canCurrentlySee() == false)
  {
    return;
  }

  /// @todo Handle field-of-view here.
  ///       Field of view for an DynamicEntity can be:
  ///          * NARROW (90 degrees straight ahead)
  ///		   * WIDE (180 degrees in facing direction)
  ///          * FRONTBACK (90 degrees ahead/90 degrees back)
  ///          * FULL (all 360 degrees)
  for (int n = 1; n <= 8; ++n)
  {
    do_recursive_visibility(n);
  }
}

MapMemoryChunk const& Entity::getMemoryAt(IntVec2 coords) const
{
  static MapMemoryChunk null_memory_chunk{ "???", GAME.getGameClock() };

  if (this->getMapId() == MapFactory::null_map_id)
  {
    return null_memory_chunk;
  }

  Map& game_map = GAME.maps().get(this->getMapId());
  return m_map_memory[game_map.getIndex(coords)];
}

bool Entity::move_into(EntityId new_location)
{
  MapId old_map_id = this->getMapId();
  EntityId old_location = COMPONENTS.position[m_id].parent();

  if (new_location == old_location)
  {
    // We're already there!
    return true;
  }

  if (new_location->can_contain(m_id))
  {
    if (new_location->getInventory().add(m_id))
    {
      // Try to lock our old location.
      if (old_location != EntityId::Mu())
      {
        old_location->getInventory().remove(m_id);
      }

      // Set the location to the new location.
      COMPONENTS.position[m_id] = new_location;

      MapId new_map_id = this->getMapId();
      if (old_map_id != new_map_id)
      {
        if (old_map_id != MapFactory::null_map_id)
        {
          /// @todo Save old map memory.
        }
        m_map_memory.clear();
        m_tiles_currently_seen.clear();
        if (new_map_id != MapFactory::null_map_id)
        {
          Map& new_map = GAME.maps().get(new_map_id);
          IntVec2 new_map_size = new_map.getSize();
          m_map_memory.resize(new_map_size.x * new_map_size.y);
          m_tiles_currently_seen.resize(new_map_size.x * new_map_size.y);
          /// @todo Load new map memory if it exists somewhere.
        }
      }
      this->findSeenTiles();
      //notifyObservers(Event::Updated);
      return true;
    } // end if (add to new inventory was successful)
  } // end if (can_contain is true)

  return false;
}

Inventory& Entity::getInventory()
{
  return m_inventory;
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

MapTile* Entity::getMapTile() const
{
  auto& parent = COMPONENTS.position[m_id].parent();

  if (parent == EntityId::Mu())
  {
    return _get_maptile();
  }
  else
  {
    return parent->getMapTile();
  }
}

MapId Entity::getMapId() const
{
  auto parent = COMPONENTS.position[m_id].parent();

  if (parent == EntityId::Mu())
  {
    MapTile* maptile = _get_maptile();
    if (maptile != nullptr)
    {
      return maptile->getMapId();
    }
    else
    {
      return static_cast<MapId>(0);
    }
  }
  else
  {
    return parent->getMapId();
  }
}

std::string Entity::getDisplayAdjectives() const
{
  std::string adjectives;

  if (isSubtypeOf("DynamicEntity") && (static_cast<int>(getModifiedProperty("hp", 0)) <= 0))
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

std::string Entity::getProperName() const
{
  return getModifiedProperty("proper-name", std::string());
}

void Entity::setProperName(std::string name)
{
  setBaseProperty("proper-name", name);
}

std::string Entity::getSubjectiveString(ArticleChoice articles) const
{
  std::string str;

  if (isPlayer())
  {
    if (static_cast<int>(getModifiedProperty("hp", 0)) > 0)
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
    if (static_cast<int>(getModifiedProperty("hp", 0)) > 0)
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

  EntityId location = this->getLocation();
  unsigned int quantity = this->getQuantity();

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

  owned = location->isSubtypeOf("DynamicEntity");
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

    if (getProperName().empty() == false)
    {
      suffix = tr("VERB_NAME_PP") + " " + getProperName();
    }
  }
  else
  {
    noun = std::to_string(getQuantity()) + " " + getDisplayPlural();

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
  return (GAME.getPlayer() == m_id) || (static_cast<unsigned int>(getBaseProperty("quantity", 1)) > 1);
}

std::string const& Entity::chooseVerb(std::string const& verb12,
                                        std::string const& verb3)
{
  if ((GAME.getPlayer() == m_id) || (static_cast<unsigned int>(getBaseProperty("quantity", 1)) > 1))
  {
    return verb12;
  }
  else
  {
    return verb3;
  }
}

unsigned int Entity::getMass()
{
  return static_cast<unsigned int>(getModifiedProperty("physical-mass", 0)) * 
    static_cast<unsigned int>(getBaseProperty("quantity", 1));
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
  return getModifiedProperty("opacity", Color::White);
}

bool Entity::isOpaque()
{
  return (getOpacity() == Color::White);
}

void Entity::light_up_surroundings()
{
  EntityId location = getLocation();

  if (static_cast<int>(getIntrinsic("inventory-size", 0)) != 0)
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in an entity's inventory.

    bool opaque = location->isOpaque();
    bool is_entity = this->isSubtypeOf("DynamicEntity");

    /*CLOG(DEBUG, "Entity") << "light_up_surroundings - this->type = " << this->getCategory() <<
      ", location->type = " << location->getCategory() <<
      ", location->opaque = " << opaque <<
      ", this->isSubtypeOf(\"DynamicEntity\") = " << is_entity;*/

    //if (!isOpaque() || isWielding(light) || isWearing(light))
    if (!opaque || is_entity)
    {
      auto& inventory = getInventory();
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
  if ((location != EntityId::Mu()) && this->getModifiedProperty("lit", false))
  {
    location->beLitBy(this->getId());
  }
}

void Entity::beLitBy(EntityId light)
{
  bool result = call_lua_function("on_lit_by", light, true);
  if (result)
  {
    //notifyObservers(Event::Updated);
  }

  EntityId location = getLocation();

  if (getLocation() == EntityId::Mu())
  {
    GAME.maps().get(getMapId()).addLight(light);
  }
  else
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in the player's inventory.

    bool opaque = this->isOpaque();
    bool is_entity = this->isSubtypeOf("DynamicEntity");

    //if (!isOpaque() || isWielding(light) || isWearing(light))
    if (!opaque || is_entity)
    {
      location->beLitBy(light);
    }
  }
}

/// @todo Make this into an Action.
void Entity::spill()
{
  Inventory& inventory = getInventory();
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
      if (parent->can_contain(entity))
      {
        // Try to move this into the Entity's location.
        success = entity->move_into(parent);
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

      } // end if (can_contain)
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

  if (static_cast<int>(getIntrinsic("inventory-size", 0)) != 0)
  {
    // Spill the contents of this Entity into the Entity's location.
    spill();
  }

  if (old_location != EntityId::Mu())
  {
    old_location->getInventory().remove(m_id);
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

bool Entity::canBeObjectOfAction(Actions::Action & action)
{
  return canBeObjectOfAction(action.getType());
}

bool Entity::canBeObjectOfAction(std::string action)
{
  return getIntrinsic("can-be-object-of-" + action, false);
}


/// @todo Have the script return an optional reason if an action can't be done.
bool Entity::canHaveActionDoneBy(EntityId entity, std::string action)
{
  return canBeObjectOfAction(action) &&
    call_lua_function("can_have_action_" + action + "_done_by", entity, false);
}

bool Entity::canHaveActionDoneBy(EntityId entity, Actions::Action& action)
{
  return canHaveActionDoneBy(entity, action.getType());
}

bool Entity::is_miscible_with(EntityId entity)
{
  return call_lua_function("is_miscible_with", entity, false);
}

BodyPart Entity::is_equippable_on() const
{
  return BodyPart::Count;
}

bool Entity::process_involuntary_actions()
{
  // Get a copy of the Entity's inventory.
  // This is because entities can be deleted/removed from the inventory
  // over the course of processing them, and this could invalidate the
  // iterator.
  Inventory temp_inventory{ m_inventory };

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
  Inventory temp_inventory{ m_inventory };

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

bool Entity::be_object_of(Actions::Action& action, EntityId subject)
{
  return call_lua_function("on_object_of_" + action.getType(), subject, true);
}

bool Entity::be_object_of(Actions::Action& action, EntityId subject, EntityId target)
{
  return call_lua_function("on_object_of_" + action.getType(), { subject, target }, true);
}

bool Entity::be_object_of(Actions::Action& action, EntityId subject, Direction direction)
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
  if (other->getCategory() != getCategory())
  {
    return false;
  }

  // Entities with inventories can never merge.
  if ((static_cast<int>(getIntrinsic("inventory-size", 0)) != 0) ||
      (static_cast<int>(other->getIntrinsic("inventory-size", 0)) != 0))
  {
    return false;
  }

  // If the entities have the exact same properties, merge is okay.
  /// @todo Handle default properties. Right now, if a property was
  ///       queried on a Entity and pulls the default, but it was NOT queried
  ///       on the second entity, the property dictionaries will NOT match.
  ///       I have not yet found a good solution to this problem.
  auto& our_properties = this->m_properties;
  auto& other_properties = other->m_properties;

  if ((this->m_properties) == (other->m_properties))
  {
    return true;
  }

  return false;
}

bool Entity::can_contain(EntityId entity)
{
  int inventory_size = getIntrinsic("inventory-size", 0);
  if (inventory_size == 0)
  {
    return false;
  }
  else if (getInventory().count() >= inventory_size)
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

void Entity::setLocation(EntityId target)
{
  COMPONENTS.position[m_id] = target;
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
  int counter_busy = getBaseProperty("counter-busy", 0);
  bool entity_updated = false;

  // Is this an entity that is now dead?
  if (isSubtypeOf("DynamicEntity") && (static_cast<int>(getModifiedProperty("hp", 0)) <= 0))
  {
    // Did the entity JUST die?
    if (!getModifiedProperty("dead", false))
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
        getCategory() << "): Involuntary Action " <<
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
  int counter_busy = getBaseProperty("counter-busy", 0);

  // Is this an entity that is now dead?
  if (isSubtypeOf("DynamicEntity") && (static_cast<int>(getModifiedProperty("hp", 0)) <= 0))
  {
    // Did the entity JUST die?
    if (!getModifiedProperty("dead", false))
    {
      // Perform the "die" action.
      // (This sets the "dead" property and clears out any pending actions.)
      std::unique_ptr<Actions::Action> dieAction(NEW Actions::ActionDie(getId()));
      this->queueInvoluntaryAction(std::move(dieAction));
    }
  }

  // If this entity is busy...
  if (counter_busy > 0)
  {
    // Decrement busy counter.
    addToBaseProperty("counter-busy", -1);
  }
  // Otherwise if there are pending actions...
  else if (!m_pending_voluntary_actions.empty())
  {
    bool entity_updated = false;

    // While actions are pending and we're not busy...
    while (!m_pending_voluntary_actions.empty() && counter_busy == 0)
    {
      // Process the front action.
      // @todo Find a way to update the entity_updated variable.
      std::unique_ptr<Actions::Action>& action = m_pending_voluntary_actions.front();
      bool action_done = action->process({});
      if (action_done)
      {
        CLOG(TRACE, "Entity") << "Entity " <<
          getId() << " (" <<
          getCategory() << "): Action " <<
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

MapMemory& Entity::get_map_memory()
{
  return m_map_memory;
}

void Entity::do_recursive_visibility(int octant,
                                    int depth,
                                    float slope_A,
                                    float slope_B)
{
  Assert("Entity", octant >= 1 && octant <= 8, "Octant" << octant << "passed in is not between 1 and 8 inclusively");
  IntVec2 new_coords;
  //int x = 0;
  //int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (isInsideAnotherEntity())
  {
    return;
  }

  MapTile* tile = getMapTile();
  IntVec2 tile_coords = tile->getCoords();
  Map& game_map = GAME.maps().get(getMapId());

  static const int mv = 128;
  static constexpr int mw = (mv * mv);

  std::function< bool(RealVec2, RealVec2, float) > loop_condition;
  Direction dir;
  std::function< float(RealVec2, RealVec2) > recurse_slope;
  std::function< float(RealVec2, RealVec2) > loop_slope;

  switch (octant)
  {
    case 1:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
      break;

    case 2:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Northeast.half(), b); };
      break;

    case 3:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
      break;

    case 4:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      break;

    case 5:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
      break;

    case 6:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Southwest.half(), b); };
      break;

    case 7:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
      break;

    case 8:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      break;

    default:
      break;
  }

  while (loop_condition(to_v2f(new_coords), to_v2f(tile_coords), slope_B))
  {
    if (calc_vis_distance(new_coords, tile_coords) <= mw)
    {
      if (game_map.tileIsOpaque(new_coords))
      {
        if (!game_map.tileIsOpaque(new_coords + (IntVec2)dir))
        {
          do_recursive_visibility(octant, depth + 1, slope_A, recurse_slope(to_v2f(new_coords), to_v2f(tile_coords)));
        }
      }
      else
      {
        if (game_map.tileIsOpaque(new_coords + (IntVec2)dir))
        {
          slope_A = loop_slope(to_v2f(new_coords), to_v2f(tile_coords));
        }
      }
      m_tiles_currently_seen[game_map.getIndex(new_coords)] = true;

      MapMemoryChunk new_memory{ game_map.getTile(new_coords).getTileType(),
                                 GAME.getGameClock() };
      m_map_memory[game_map.getIndex(new_coords)] = new_memory;
    }
    new_coords -= (IntVec2)dir;
  }
  new_coords += (IntVec2)dir;

  if ((depth < mv) && (!game_map.getTile(new_coords).isOpaque()))
  {
    do_recursive_visibility(octant, depth + 1, slope_A, slope_B);
  }
}

MapTile* Entity::_get_maptile() const
{
  return m_map_tile;
}