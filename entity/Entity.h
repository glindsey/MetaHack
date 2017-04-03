#pragma once

#include "stdafx.h"

#include "json.hpp"
using json = ::nlohmann::json;

#include "actions/Action.h"
#include "types/BodyPart.h"
#include "types/Direction.h"
#include "types/GameObject.h"
#include "types/Gender.h"
#include "lua/LuaObject.h"
#include "types/MapMemoryChunk.h"
#include "maptile/MapTile.h"
#include "metadata/Metadata.h"
#include "properties/ModifiablePropertyDictionary.h"
#include "Subject.h"
#include "entity/EntityId.h"
#include "entity/EntityPool.h"

// Forward declarations
class AIStrategy;
class DynamicEntity;
class MapTile;
class Inventory;
class EntityId;
class Entity;

// Using declarations.
using BodyLocationMap = std::unordered_map<BodyLocation, EntityId>;
using BodyLocationPair = std::pair<BodyLocation, EntityId>;

using MapMemory = std::vector<MapMemoryChunk>;
using TilesSeen = boost::dynamic_bitset<size_t>; // size_t gets rid of 64-bit compile warning
using ActionQueue = std::deque< std::unique_ptr<Actions::Action> >;

// Associated enum classes to aid in parameter legibility.
enum class ArticleChoice
{
  Indefinite,
  Definite
};

enum class UsePossessives
{
  No,
  Yes
};

/// Entity is any object in the game, animate or not.
/// @todo Still a LOT of cruft in this class that should be refactored out,
///       e.g. the wearing/wielding stuff.
class Entity
  :
  public GameObject,
  public Subject
{
  friend class AIStrategy;
  friend class EntityPool;

public:
  virtual ~Entity();

  /// Queue an action for this DynamicEntity to perform.
  /// @param action The Action to queue. The Action is MOVED when queued,
  ///               e.g. pAction will be `nullptr` after queuing.
  void queueAction(std::unique_ptr<Actions::Action> action);

  /// Queue an action for this DynamicEntity to perform.
  /// @param p_action The Action to queue. The queue takes ownership of the
  ///                 pointer.
  void queueAction(Actions::Action* p_action);

  /// Queue an involuntary action for this DynamicEntity to perform.
  /// @param action The Action to queue. The Action is MOVED when queued,
  ///               e.g. pAction will be `nullptr` after queuing.
  void queueInvoluntaryAction(std::unique_ptr<Actions::Action> action);

  /// Queue an involuntary action for this DynamicEntity to perform.
  /// The action is pushed to the FRONT of the queue, so it is next to be
  /// popped.
  /// @param p_action The Action to queue. The queue takes ownership of the
  ///                 pointer.
  void queueInvoluntaryAction(Actions::Action* p_action);

  /// Return whether there is any action pending for this DynamicEntity.
  bool anyActionIsPending() const;

  /// Return whether there is a voluntary action pending for this DynamicEntity.
  bool voluntaryActionIsPending() const;

  /// Return whether there is an involuntary action pending for this DynamicEntity.
  bool involuntaryActionIsPending() const;

  /// Return whether there is an action currently in progress for this DynamicEntity.
  bool actionIsInProgress();

  /// Clear all pending actions in this Entity's queues.
  void clearAllPendingActions();

  /// Clear all pending voluntary actions in this Entity's queue.
  void clearPendingVoluntaryActions();

  /// Clear all pending involuntary actions in this Entity's queue.
  void clearPendingInvoluntaryActions();

  /// Get the entity being wielded with the specified bodypart, if any.
  EntityId getWieldingIn(BodyLocation& location);

  /// Returns true if this entity is the current player.
  /// By default, returns false. Overridden by DynamicEntity class.
  virtual bool isPlayer() const;

  std::string const& getType() const;
  std::string getParentType() const;

  bool isSubtypeOf(std::string that_type) const;

  /// Return whether a Entity is wielded by this DynamicEntity.
  /// This is used by InventoryArea to show wielded status.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is wielded by the DynamicEntity.
  bool isWielding(EntityId entity);

  /// Return whether a Entity is wielded by this DynamicEntity.
  /// This is used by InventoryArea to show wielded status.
  /// @param[in] entity Entity to check
  /// @param[out] number Body part it is wielded with.
  /// @return true if the Entity is wielded by the DynamicEntity.
  bool isWielding(EntityId entity, BodyLocation& location);

  /// Return whether a Entity is equipped (worn) by this DynamicEntity.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is being worn.
  bool isWearing(EntityId entity);

  /// Return whether a Entity is being worn by this DynamicEntity.
  /// @param[in] entity Entity to check
  /// @param[out] location of the worn Entity, if worn
  /// @return true if the Entity is being worn.
  bool isWearing(EntityId entity, BodyLocation& location);

  /// Return whether a Entity is within reach of the DynamicEntity.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is in the DynamicEntity's inventory or is at the
  ///         same location as the DynamicEntity, false otherwise.
  bool canReach(EntityId entity);

  /// Return whether a Entity is adjacent to this DynamicEntity.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is at the same place or adjacent to this DynamicEntity, false otherwise.
  bool isAdjacentTo(EntityId entity);

  /// Get an intrinsic of this Entity.
  /// If the intrinsic is not found, returns the default value.
  /// @param key            Name of the intrinsic to get.
  /// @param default_value  Default value to use.
  /// @return The intrinsic (or default) value for that key.
  json getIntrinsic(std::string key, json default_value) const;
  
  /// Get a base property of this Entity.
  /// If the base property is not found, the method falls back upon the
  /// intrinsic for that property. If IT is missing, it falls back to the
  /// default value.
  /// @param key            Name of the property to get.
  /// @param default_value  Default value to use.
  /// @return The property (or default) value for that key.
  json getBaseProperty(std::string key, json default_value) const;
  
  /// Sets a base property of this Entity.
  /// If the base property is not found, it is created.
  ///
  /// @param key    Key of the property to set.
  /// @param value  Value to set the property to.
  /// @return Boolean indicating whether the property previously existed.
  bool setBaseProperty(std::string key, json value);
  
  /// Adds to a base property of this Entity.
  /// If the base property is not found, it is created.
  /// @param key    Key of the property to set.
  /// @param value  Value to add to the property.
  void addToBaseProperty(std::string key, json add_value);
  
  /// Get a modified property of this Entity.
  /// If the modified property is not found, the method falls back upon the
  /// base value for that property (if any).
  /// @param key            Name of the property to get.
 
  /// @param default_value  Default value to use, if any.
  /// @return The modified (or base) property value for that key.
  json getModifiedProperty(std::string key, json default_value) const;
  
  /// Add a property modifier to this Entity.
  /// @param  key   Name of property to modify.
  /// @param  id    ID of Entity that is responsible for modifying it.
  /// @param  info  Info for this modifier, as a PropertyModifierInfo struct.
  ///
  /// @see ModifiablePropertyDictionary::addModifier
  ///
  /// @return True if the function was added; false if it already existed.
  bool addModifier(std::string key, EntityId id, PropertyModifierInfo const& info);
  
  /// Remove all modifier functions for a given key and entity ID.
  /// @param  key               Name of property to modify.
  /// @param  id                ID of Entity that is responsible for modifying it.
  ///
  /// @return The number of modifiers erased.
  size_t removeModifier(std::string key, EntityId id);
  
  /// Get the quantity this entity represents.
  unsigned int getQuantity() const;

  /// Set the quantity this entity represents.
  void setQuantity(unsigned int quantity);

  /// Return a reference to this entity.
  EntityId getId() const;

  /// Return the root location of this Entity, traversing up the location
  /// chain until EntityId::Mu() is reached.
  EntityId getRootLocation() const;

  /// Return the location of this entity.
  EntityId getLocation() const;

  /// Return whether the DynamicEntity can see the requested Entity.
  bool canSee(EntityId entity);

  /// Return whether the DynamicEntity can see the requested tile.
  bool canSee(IntVec2 coords);

  /// Find out which tiles on the map can be seen by this DynamicEntity.
  /// In the process, tiles in the DynamicEntity's visual memory are updated.
  /// This method uses a recursive raycasting algorithm to figure out what
  /// can be seen at a particular position.
  void findSeenTiles();

  /// Get the remembered tile type at the specified coordinates.
  MapMemoryChunk const& getMemoryAt(IntVec2 coords) const;
   
  void setWielded(EntityId entity, BodyLocation location);

  void setWorn(EntityId entity, BodyLocation location);

  /// Return whether this DynamicEntity can currently see.
  /// @todo Implement blindness counter, blindness due to wearing blindfold,
  ///       et cetera.
  bool canCurrentlySee();

  /// Return whether this DynamicEntity can currently move.
  /// @todo Implement paralysis counter, and/or other reasons to be immobile.
  bool canCurrentlyMove();

  void setGender(Gender gender);

  Gender getGender() const;

  Gender getGenderOrYou() const;

  /// Get the number of a particular body part the DynamicEntity has.
  unsigned int getBodypartNumber(BodyPart part) const;

  /// Get the appropriate body part name for the DynamicEntity.
  std::string getBodypartName(BodyPart part) const;

  /// Get the appropriate body part plural for the DynamicEntity.
  std::string getBodypartPlural(BodyPart part) const;

  /// Get the appropriate description for a body part.
  /// This takes the body part name and the number referencing the particular
  /// part and comes up with a description.
  /// For example, for most creatures with two hands, hand #0 will be the
  /// "right hand" and hand #1 will be the "left hand".
  /// In most cases the default implementation here will work, but if a
  /// creature has (for example) a strange configuration of limbs this can be
  /// overridden.
  std::string getBodypartDescription(BodyLocation location);

  /// Returns true if a particular Action can be performed on this Entity by
  /// the specified Entity.
  /// @param entity  The Entity doing the Action.
  /// @param action Reference to the Action to perform.
  /// @return true if the Action can be performed, false otherwise.
  bool canHaveActionDoneBy(EntityId entity, Actions::Action& action);

  /// Returns a reference to the inventory.
  Inventory& getInventory();

  /// Returns true if this entity is inside another Entity.
  bool isInsideAnotherEntity() const;

  /// Get the MapTile this entity is on, or nullptr if not on a map.
  MapTile* getMapTile() const;

  /// Return the MapId this Entity is currently on, or 0 if not on a map.
  MapId getMapId() const;

  /// Return this object's adjective qualifiers (such as "fireproof", "waterproof", etc.)
  std::string getDisplayAdjectives() const;

  /// Return this object's name.
  std::string getDisplayName() const;

  /// Return this object's plural.
  std::string getDisplayPlural() const;

  /// Get the entity's proper name (if any).
  std::string getProperName() const;

  /// Set this entity's proper name.
  void setProperName(std::string name);

  /// Return a string that identifies this entity, in the subjective case.
  /// If it IS the player, it'll return "you".
  /// Otherwise it calls getDescriptiveString().
  ///
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  std::string getSubjectiveString(ArticleChoice articles = ArticleChoice::Definite) const;

  /// Return a string that identifies this entity, in the objective case.
  /// If it IS the player, it'll return "you".
  /// Otherwise it calls getDescriptiveString().
  ///
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  ///
  /// @note In English this has the same results as 
  /// getSubjectiveString(), but _this will not be the case
  /// in all languages_.
  std::string getObjectiveString(ArticleChoice articles = ArticleChoice::Definite) const;

  /// Return a string that identifies this entity, 
  /// If it matches the object passed in as "other", it'll return
  /// the appropriate reflexive pronoun ("yourself", "itself", etc.).
  /// Otherwise it calls getDescriptiveString().
  ///
  /// @param other      The "other" to compare to.
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  std::string getReflexiveString(EntityId other, ArticleChoice articles = ArticleChoice::Definite) const;

  /// Return a string that identifies this entity.
  /// Returns "the/a/an" and a description of the entity, such as
  /// "the chair".
  /// If it is carried by the player, and possessives = true, it'll
  /// return "your (entity)".
  /// Likewise, if it is carried by another DynamicEntity it'll return
  /// "(DynamicEntity)'s (entity)".
  /// @todo Make localizable. (How? Use Lua scripts maybe?)
  ///
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  /// @param possessives  Choose whether to use possessive articles when appropriate.
  ///                     Defaults to using them.
  std::string getDescriptiveString(ArticleChoice articles = ArticleChoice::Definite,
                                   UsePossessives possessives = UsePossessives::Yes) const;

  /// Choose the proper possessive form for a string passed in.
  /// For a Entity, this is simply "the foo", as Entities cannot own entities.
  /// This function checks to see if this Entity is currently designated as
  /// the player.  If so, it returns "your foo".  If not, it returns getName() + "'s foo".
  /// @todo Make localizable. (How? Use Lua scripts maybe?)
  ///
  /// @param owned      String name of the thing that is possessed.
  /// @param adjectives Optional adjectives to add.
  ///
  /// @note If you want a possessive pronoun like his/her/its/etc., use
  /// getPossessiveAdjective().
  std::string getPossessiveString(std::string owned, std::string adjectives = "");

  Color getOpacity() const;

  /// Return true if a third-person verb form should be used.
  /// This function checks to see if this Entity is currently designated as
  /// the player, or has a quantity greater than zero.
  /// If so, it returns true; otherwise, it returns false.
  /// @todo See if this is different for languages other than English.
  ///       I'm guessing the answer is "yes, yes it is".
  bool isThirdPerson();

  /// Choose which verb form to use based on first/second/third person.
  /// This function checks to see if this Entity is currently designated as
  /// the player, or has a quantity greater than zero.
  /// If so, it returns the string passed as verb2; otherwise, it returns the
  /// string passed as verb3.
  /// @param verb2 The second person or plural verb form, such as "shake"
  /// @param verb3 The third person verb form, such as "shakes"
  std::string const& chooseVerb(std::string const& verb2,
                                std::string const& verb3);

  /// Return this entity's mass.
  unsigned int getMass();

  /// @addtogroup Pronouns
  /// @todo Make localizable. (How? Use Lua scripts maybe?)
  /// @{

  /// Get the appropriate subject pronoun for the Entity.
  std::string const& getSubjectPronoun() const;

  /// Get the appropriate object pronoun for the Entity.
  std::string const& getObjectPronoun() const;

  /// Get the appropriate reflexive pronoun for the Entity.
  std::string const& getReflexivePronoun() const;

  /// Get the appropriate possessive adjective for the Entity.
  std::string const& getPossessiveAdjective() const;

  /// Get the appropriate possessive pronoun for the Entity.
  std::string const& getPossessivePronoun() const;

  /// @}


  /// Simple check to see if a Entity is opaque.
  bool isOpaque();

  /// Provide light to this Entity's surroundings.
  /// If Entity is not opaque, calls light_up_surroundings() for each Entity
  /// in its inventory.
  void light_up_surroundings();

  /// Receive light from the specified light source.
  /// The default behavior is to pass the light source to the location if
  /// this Entity is opaque.
  void beLitBy(EntityId light);

  /// Spill the contents of this Entity out into the location of the Entity.
  void spill();

  /// Attempt to destroy this Entity.
  void destroy();

  /// Attempt to move this Entity into a location.
  bool move_into(EntityId new_location);

  /// Return whether or not this entity can be mixed with another Entity.
  /// The default behavior for this is to return false.
  bool is_miscible_with(EntityId entity);

  /// Return the body part this entity is equippable on.
  /// If entity is not equippable, return BodyPart::Count.
  BodyPart is_equippable_on() const;

  /// Process involuntary actions of this Entity and its inventory for a single tick.
  bool process_involuntary_actions();

  /// Process voluntary actions of this Entity and its inventory for a single tick.
  bool process_voluntary_actions();

  /// Perform an action when this entity dies.
  /// @return If this function returns false, the death is avoided.
  /// @warning The function must reset whatever caused the death in the
  ///          first place, or the Entity will just immediately die again
  ///          on the next call to process()!
  bool perform_action_died();

  /// Perform an action when this entity collides with another entity.
  void perform_action_collided_with(EntityId entity);

  /// Perform an action when this entity collides with a wall.
  void perform_action_collided_with_wall(Direction d, std::string tile_type);

  /// Perform the effects of being the subject of a particular action.
  /// This is typically called on intransitive verbs (ones that do not take
  /// an object); "die" is a good example. You can't "die something" (though
  /// you can "dye something" but that's beside the point), so this would be
  /// an appropriate method to call.
  /// 
  /// @param action   The action to be the target of.
  /// @return Bool indicating whether the action succeeded.
  bool perform_intransitive_action(Actions::Action& action);

  /// Perform the effects of being a object of a particular action.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @return Bool indicating whether the action succeeded.
  bool be_object_of(Actions::Action& action, EntityId subject);

  /// Perform the effects of being a object of an action with a target.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @param target   The target of the action.
  /// @return Bool indicating whether the action succeeded.
  bool be_object_of(Actions::Action& action, EntityId subject, EntityId target);

  /// Perform the effects of being a object of an action with a direction.
  /// @param action     The action to be the target of.
  /// @param subject    The subject performing the action.
  /// @param direction  The direction of the action.
  /// @return Bool indicating whether the action succeeded.
  bool be_object_of(Actions::Action& action, EntityId subject, Direction direction);

  /// Perform an action when this entity is hit by an attack.
  bool perform_action_hurt_by(EntityId subject);

  /// Perform an action when this entity is used to hit a target.
  /// This action executes when the entity is wielded by an entity, and an
  /// attack successfully hits its target.  It is a side-effect in addition
  /// to the damage done by DynamicEntity::attack(entity).
  /// @see DynamicEntity::attack
  /// @note Is there a better name for this? Current name sounds like the
  ///       object is the target, instead of the implement.
  bool perform_action_attacked_by(EntityId subject, EntityId target);

  /// Perform an action when this entity is de-equipped (taken off).
  /// If this function returns false, the action is aborted.
  bool perform_action_deequipped_by(EntityId actor, BodyLocation& location);

  /// Perform an action when this entity is equipped.
  /// If this function returns false, the action is aborted.
  bool perform_action_equipped_by(EntityId actor, BodyLocation& location);
  
  /// Returns whether the Entity can merge with another Entity.
  /// Calls an overridden subclass function.
  bool can_merge_with(EntityId other) const;

  /// Returns whether the Entity can hold a certain entity.
  /// If Entity's inventory size is 0, returns false.
  /// Otherwise, calls Lua function "can_contain()" for the Entity's type.
  /// @param entity Entity to check.
  /// @return Bool specifying whether the entity can be held here.
  bool can_contain(EntityId entity);

  /// Syntactic sugar for calling call_lua_function().
  json call_lua_function(std::string function_name,
                         json const& args,
                         json const& default_result);

  json call_lua_function(std::string function_name,
                         json const& args);

  json call_lua_function(std::string function_name,
                         json const& args,
                         json const& default_result) const;

  json call_lua_function(std::string function_name,
                         json const& args) const;

  /// Get a const reference to this tile's metadata.
  Metadata const & getMetadata() const;

  virtual std::unordered_set<EventID> registeredEvents() const override;

protected:
  /// Named Constructor
  Entity(GameState& game, Metadata& metadata, EntityId ref);

  /// Floor Constructor
  Entity(GameState& game, MapTile* map_tile, Metadata& metadata, EntityId ref);

  /// Clone Constructor
  Entity(Entity const& original, EntityId ref);

  /// Initializer; called by all constructors.
  void initialize();

  /// Get a reference to this DynamicEntity's map memory.
  MapMemory& get_map_memory();

  /// Perform the recursive visibility scan for an octant.
  /// Used by findSeenTiles.
  void do_recursive_visibility(int octant,
                               int depth = 1,
                               float slope_A = 1,
                               float slope_B = 0);

  /// Set the location of this entity.
  /// Does no checks, nor does it update the source/target inventories.
  /// Those are the responsibility of the caller.
  void setLocation(EntityId target);

  /// Gets this location's maptile.
  virtual MapTile* _get_maptile() const;

  /// Process this Entity's involuntary actions for a single tick.
  /// Voluntary actions are only processed when the Entity is not busy.
  /// The function returns false to indicate to its parent that it no longer
  /// exists and should be deleted.
  /// @return true if the Entity continues to exist after the tick;
  ///         false if the Entity ceases to exist.
  virtual bool _process_own_involuntary_actions();

  /// Process this Entity's voluntary actions for a single tick.
  /// Involuntary actions are processed even if the Entity is busy.
  /// The function returns false to indicate to its parent that it no longer
  /// exists and should be deleted.
  /// @return true if the Entity continues to exist after the tick;
  ///         false if the Entity ceases to exist.
  virtual bool _process_own_voluntary_actions();

private:
  /// Reference to game state.
  GameState& m_game;

  /// Reference to this Entity's metadata.
  Metadata& m_metadata;

  /// Property dictionary.
  /// Defined as mutable, because a "get" method can cache a default value
  /// for a key if one doesn't already exist.
  ModifiablePropertyDictionary mutable m_properties;

  /// Reference to this Entity.
  EntityId m_id;

  /// Reference to this Entity's location.
  EntityId m_location;

  /// If this Entity is a Floor, pointer to the MapTile it is on.
  MapTile* m_map_tile;

  /// This Entity's inventory.
  Inventory m_inventory;

  /// Gender of this entity.
  Gender m_gender = Gender::None;

  /// DynamicEntity's spacial memory of map tiles.
  /// @todo Regarding memory, it would be AWESOME if it could fade out
  ///       VERY gradually, over a long period of time. Seeing it would
  ///       reset the memory counter to 0, or possibly just add a large
  ///       amount to the counter so that places you see more frequently
  ///       stay in your mind longer.
  MapMemory m_map_memory;

  /// Bitset for tiles currently seen.
  /// This deals with tiles observed at this particular instant.
  TilesSeen m_tiles_currently_seen;

  /// Queue of pending involuntary actions to be performed.
  ActionQueue m_pending_involuntary_actions;

  /// Queue of pending voluntary actions to be performed.
  ActionQueue m_pending_voluntary_actions;

  /// Map of items wielded.
  BodyLocationMap m_wielded_items;

  /// Map of entities worn.
  BodyLocationMap m_equipped_items;

  /// Outline color for walls when drawing on-screen.
  static Color const wall_outline_color_;
};
