#ifndef THING_H
#define THING_H

#include "stdafx.h"

#include "Action.h"
#include "ActionResult.h"
#include "BodyPart.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "GameObject.h"
#include "Gender.h"
#include "LuaObject.h"
#include "MapMemoryChunk.h"
#include "MapTile.h"
#include "Metadata.h"
#include "ModifiablePropertyDictionary.h"
#include "Observable.h"
#include "EntityId.h"
#include "EntityPool.h"

// Forward declarations
class AIStrategy;
class DynamicEntity;
class MapTile;
class Inventory;
class EntityId;
class Entity;

// Using declarations.
using WieldingMap = std::unordered_map<unsigned int, EntityId>;
using WieldingPair = std::pair<unsigned int, EntityId>;

using WearingMap = std::unordered_map<WearLocation, EntityId>;
using WearingPair = std::pair<WearLocation, EntityId>;

using MapMemory = std::vector<MapMemoryChunk>;
using TilesSeen = boost::dynamic_bitset<size_t>; // size_t gets rid of 64-bit compile warning
using ActionQueue = std::deque< std::unique_ptr<Action> >;

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
  public Observable
{
  friend class AIStrategy;
  friend class EntityPool;

public:
  virtual ~Entity();

  /// Queue an action for this DynamicEntity to perform.
  /// pAction The Action to queue. The Action is MOVED when queued,
  ///         e.g. pAction will be `nullptr` after queuing.
  void queue_action(std::unique_ptr<Action> pAction);

  /// Return whether there is an action pending for this DynamicEntity.
  bool action_is_pending() const;

  /// Return whether there is an action currently in progress for this DynamicEntity.
  bool action_is_in_progress();

  /// Get the entity being wielded in the specified hand, if any.
  EntityId get_wielding_in(unsigned int & hand);

  /// Returns true if this entity is the current player.
  /// By default, returns false. Overridden by DynamicEntity class.
  virtual bool is_player() const;

  std::string const& get_type() const;
  std::string const& get_parent_type() const;

  bool is_subtype_of(std::string that_type) const;

  /// Return whether a Entity is wielded by this DynamicEntity.
  /// This is used by InventoryArea to show wielded status.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is wielded by the DynamicEntity.
  bool is_wielding(EntityId entity);

  /// Return whether a Entity is wielded by this DynamicEntity.
  /// This is used by InventoryArea to show wielded status.
  /// @param[in] entity Entity to check
  /// @param[out] number Hand number it is wielded in.
  /// @return true if the Entity is wielded by the DynamicEntity.
  bool is_wielding(EntityId entity, unsigned int& number);

  /// Return whether a Entity is equipped (worn) by this DynamicEntity.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is being worn.
  bool has_equipped(EntityId entity);

  /// Return whether a Entity is being worn by this DynamicEntity.
  /// @param[in] entity Entity to check
  /// @param[out] location of the worn Entity, if worn
  /// @return true if the Entity is being worn.
  bool has_equipped(EntityId entity, WearLocation& location);

  /// Return whether a Entity is within reach of the DynamicEntity.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is in the DynamicEntity's inventory or is at the
  ///         same location as the DynamicEntity, false otherwise.
  bool can_reach(EntityId entity);

  /// Return whether a Entity is adjacent to this DynamicEntity.
  /// @param[in] entity Entity to check
  /// @return true if the Entity is at the same place or adjacent to this DynamicEntity, false otherwise.
  bool is_adjacent_to(EntityId entity);

  /// Die.
  /// @return True if the Entity died, false if the death was avoided.
  bool do_die();

  /// Get an intrinsic of this Entity.
  /// If the intrinsic is not found, returns the default value.
  /// @param key            Name of the intrinsic to get.
  /// @param default_value  Default value to use, if any.
  /// @return The intrinsic (or default) value for that key.
  template<typename T>
  T get_intrinsic(std::string key, T default_value = T()) const
  {
    return m_metadata.get_intrinsic<T>(key, default_value);
  }

  /// Get a base property of this Entity.
  /// If the base property is not found, the method falls back upon the
  /// intrinsic for that property (if any).
  /// @param key            Name of the property to get.
  /// @param default_value  Default value to use, if any.
  /// @return The property (or default) value for that key.
  template<typename T>
  T get_base_property(std::string key, T default_value = T()) const
  {
    if (m_properties.contains(key))
    {
      return m_properties.get<T>(key);
    }
    else
    {
      T value = m_metadata.get_intrinsic<T>(key, default_value);
      m_properties.set<T>(key, value);
      return value;
    }
  }

  /// Sets a base property of this Entity.
  /// If the base property is not found, it is created.
  ///
  /// @param key    Key of the property to set.
  /// @param value  Value to set the property to.
  /// @return Boolean indicating whether the property previously existed.
  template<typename T>
  bool set_base_property(std::string key, T value)
  {
    bool existed = m_properties.contains(key);
    m_properties.set<T>(key, value);

    return existed;
  }

  /// Adds to a base property of this Entity.
  /// If the base property is not found, it is created.
  /// @param key    Key of the property to set.
  /// @param value  Value to add to the property.
  template<typename T>
  void add_to_base_property(std::string key, T add_value)
  {
    T existing_value = m_properties.get<T>(key);
    T new_value = existing_value + add_value;
    m_properties.set<T>(key, new_value);
  }

  /// Get a modified property of this Entity.
  /// If the modified property is not found, the method falls back upon the
  /// base value for that property (if any).
  /// @param key            Name of the property to get.
  /// @param default_value  Default value to use, if any.
  /// @return The modified (or base) property value for that key.
  template<typename T>
  T get_modified_property(std::string key, T default_value = T()) const
  {
    if (!m_properties.contains(key))
    {
      T value = m_metadata.get_intrinsic<T>(key, default_value);
      m_properties.set<T>(key, value);
    }

    return m_properties.get_modified<T>(key);
  }

  /// Add a property modifier to this Entity.
  /// @param  key               Name of property to modify.
  /// @param  id                ID of Entity that is responsible for modifying it.
  /// @param  expiration_ticks  Number of ticks until modifier is removed, or 0
  ///                           if it is never removed.
  ///
  /// @see ModifiablePropertyDictionary::add_modifier
  ///
  /// @return True if the function was added; false if it already existed.
  bool add_modifier(std::string key, EntityId id, unsigned int expiration_ticks = 0);

  /// Remove all modifier functions for a given key and entity ID.
  /// @param  key               Name of property to modify.
  /// @param  id                ID of Entity that is responsible for modifying it.
  ///
  /// @return The number of modifiers erased.
  size_t remove_modifier(std::string key, EntityId id);

  /// Get the quantity this entity represents.
  unsigned int get_quantity() const;

  /// Set the quantity this entity represents.
  void set_quantity(unsigned int quantity);

  /// Return a reference to this entity.
  EntityId get_id() const;

  /// Return the root location of this Entity, traversing up the location
  /// chain until EntityId::Mu() is reached.
  EntityId get_root_location() const;

  /// Return the location of this entity.
  EntityId get_location() const;

  /// Return whether the DynamicEntity can see the requested Entity.
  bool can_see(EntityId entity);

  /// Return whether the DynamicEntity can see the requested tile.
  bool can_see(Vec2i coords);

  /// Find out which tiles on the map can be seen by this DynamicEntity.
  /// In the process, tiles in the DynamicEntity's visual memory are updated.
  /// This method uses a recursive raycasting algorithm to figure out what
  /// can be seen at a particular position.
  void find_seen_tiles();

  /// Get the remembered tile type at the specified coordinates.
  MapMemoryChunk const& get_memory_at(Vec2i coords) const;

  ActionResult can_deequip(EntityId thing_id, unsigned int& action_time);

  /// Attempt to de-equip (remove) a entity.
  bool do_deequip(EntityId thing_id, unsigned int& action_time);

  ActionResult can_equip(EntityId thing_id, unsigned int& action_time);

  /// Attempt to equip (wear) a entity.
  bool do_equip(EntityId thing_id, unsigned int& action_time);

  void set_wielded(EntityId entity, unsigned int hand);

  void set_worn(EntityId entity, WearLocation location);

  /// Return whether this DynamicEntity can currently see.
  /// @todo Implement blindness counter, blindness due to wearing blindfold,
  ///       et cetera.
  bool can_currently_see();

  /// Return whether this DynamicEntity can currently move.
  /// @todo Implement paralysis counter, and/or other reasons to be immobile.
  bool can_currently_move();

  void set_gender(Gender gender);

  Gender get_gender() const;

  Gender get_gender_or_you() const;

  /// Get the number of a particular body part the DynamicEntity has.
  unsigned int get_bodypart_number(BodyPart part) const;

  /// Get the appropriate body part name for the DynamicEntity.
  std::string get_bodypart_name(BodyPart part) const;

  /// Get the appropriate body part plural for the DynamicEntity.
  std::string get_bodypart_plural(BodyPart part) const;

  /// Get the appropriate description for a body part.
  /// This takes the body part name and the number referencing the particular
  /// part and comes up with a description.
  /// For example, for most creatures with two hands, hand #0 will be the
  /// "right hand" and hand #1 will be the "left hand".
  /// In most cases the default implementation here will work, but if a
  /// creature has (for example) a strange configuration of limbs this can be
  /// overridden.
  std::string get_bodypart_description(BodyPart part, unsigned int number);

  /// Returns true if a particular Action can be performed on this Entity by
  /// the specified Entity.
  /// @param entity  The Entity doing the Action.
  /// @param action Reference to the Action to perform.
  /// @return true if the Action can be performed, false otherwise.
  bool can_have_action_done_by(EntityId entity, Action& action);

  /// Returns a reference to the inventory.
  Inventory& get_inventory();

  /// Returns true if this entity is inside another Entity.
  bool is_inside_another_thing() const;

  /// Get the MapTile this entity is on, or nullptr if not on a map.
  MapTile* get_maptile() const;

  /// Return the MapId this Entity is currently on, or 0 if not on a map.
  MapId get_map_id() const;

  /// Return this object's adjective qualifiers (such as "fireproof", "waterproof", etc.)
  std::string get_display_adjectives() const;

  /// Return this object's name.
  std::string get_display_name() const;

  /// Return this object's plural.
  std::string get_display_plural() const;

  /// Get the entity's proper name (if any).
  std::string get_proper_name() const;

  /// Set this entity's proper name.
  void set_proper_name(std::string name);

  /// Return a string that identifies this entity.
  /// If it IS the player, it'll return "you".
  /// Otherwise it calls get_identifying_string().
  ///
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  std::string get_you_or_identifying_string(ArticleChoice articles = ArticleChoice::Definite) const;

  /// Return a string that identifies this entity.
  /// If it matches the object passed in as "other", it'll return
  /// the appropriate reflexive pronoun ("yourself", "itself", etc.).
  /// Otherwise it calls get_identifying_string().
  ///
  /// @param other      The "other" to compare to.
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  std::string get_self_or_identifying_string(EntityId other, ArticleChoice articles = ArticleChoice::Definite) const;

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
  std::string get_identifying_string(ArticleChoice articles = ArticleChoice::Definite,
                                       UsePossessives possessives = UsePossessives::Yes) const;

  /// Choose the proper possessive form
  /// For a Entity, this is simply "the", as Entities cannot own entities.
  /// This function checks to see if this Entity is currently designated as
  /// the player.  If so, it returns "your".  If not, it returns get_name() + "'s".
  /// @todo Make localizable. (How? Use Lua scripts maybe?)
  ///
  /// @note If you want a possessive pronoun like his/her/its/etc., use
  /// get_possessive_adjective().
  std::string get_possessive();

  /// Return true if a third-person verb form should be used.
  /// This function checks to see if this Entity is currently designated as
  /// the player, or has a quantity greater than zero.
  /// If so, it returns true; otherwise, it returns false.
  bool is_third_person();

  /// Choose which verb form to use based on first/second/third person.
  /// This function checks to see if this Entity is currently designated as
  /// the player, or has a quantity greater than zero.
  /// If so, it returns the string passed as verb2; otherwise, it returns the
  /// string passed as verb3.
  /// @param verb2 The second person or plural verb form, such as "shake"
  /// @param verb3 The third person verb form, such as "shakes"
  std::string const& choose_verb(std::string const& verb2,
                                   std::string const& verb3);

  /// Return this entity's mass.
  int get_mass();

  /// @addtogroup Pronouns
  /// @todo Make localizable. (How? Use Lua scripts maybe?)
  /// @{

  /// Get the appropriate subject pronoun for the Entity.
  std::string const& get_subject_pronoun() const;

  /// Get the appropriate object pronoun for the Entity.
  std::string const& get_object_pronoun() const;

  /// Get the appropriate reflexive pronoun for the Entity.
  std::string const& get_reflexive_pronoun() const;

  /// Get the appropriate possessive adjective for the Entity.
  std::string const& get_possessive_adjective() const;

  /// Get the appropriate possessive pronoun for the Entity.
  std::string const& get_possessive_pronoun() const;

  /// @}

  /// Return the coordinates of the tile representing the entity.
  Vec2u get_tile_sheet_coords(int frame);

  /// Simple check to see if a Entity is opaque.
  bool is_opaque();

  /// Provide light to this Entity's surroundings.
  /// If Entity is not opaque, calls light_up_surroundings() for each Entity
  /// in its inventory.
  void light_up_surroundings();

  /// Receive light from the specified light source.
  /// The default behavior is to pass the light source to the location if
  /// this Entity is opaque.
  void be_lit_by(EntityId light);

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

  /// Process this Entity and its inventory for a single tick.
  bool process();

  /// Perform an action when this entity dies.
  /// @return If this function returns Failure, the death is avoided.
  /// @warning The function must reset whatever caused the death in the
  ///          first place, or the Entity will just immediately die again
  ///          on the next call to process()!
  ActionResult perform_action_died();

  /// Perform an action when this entity collides with another entity.
  void perform_action_collided_with(EntityId entity);

  /// Perform an action when this entity collides with a wall.
  void perform_action_collided_with_wall(Direction d, std::string tile_type);

  /// Perform the effects of being a object of a particular action.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @return Result of the action. If Failure, the action is aborted
  ///         (if possible).
  ActionResult be_object_of(Action & action, EntityId subject);

  /// Perform the effects of being a object of an action with a target.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @param target   The target of the action.
  /// @return Result of the action. If Failure, the action is aborted
  ///         (if possible).
  ActionResult be_object_of(Action & action, EntityId subject, EntityId target);

  /// Perform the effects of being a object of an action with a direction.
  /// @param action     The action to be the target of.
  /// @param subject    The subject performing the action.
  /// @param direction  The direction of the action.
  /// @return Result of the action. If Failure, the action is aborted
  ///         (if possible).
  ActionResult be_object_of(Action & action, EntityId subject, Direction direction);

  /// Perform an action when this entity is hit by an attack.
  ActionResult perform_action_hurt_by(EntityId subject);

  /// Perform an action when this entity is used to hit a target.
  /// This action executes when the entity is wielded by an entity, and an
  /// attack successfully hits its target.  It is a side-effect in addition
  /// to the damage done by DynamicEntity::attack(entity).
  /// @see DynamicEntity::attack
  /// @note Is there a better name for this? Current name sounds like the
  ///       object is the target, instead of the implement.
  ActionResult perform_action_attacked_by(EntityId subject, EntityId target);

  /// Perform an action when this entity is de-equipped (taken off).
  /// If this function returns false, the action is aborted.
  bool perform_action_deequipped_by(EntityId actor, WearLocation& location);

  /// Perform an action when this entity is equipped.
  /// If this function returns false, the action is aborted.
  bool perform_action_equipped_by(EntityId actor, WearLocation& location);
  
  /// Returns whether the Entity can merge with another Entity.
  /// Calls an overridden subclass function.
  bool can_merge_with(EntityId other) const;

  /// Returns whether the Entity can hold a certain entity.
  /// If Entity's inventory size is 0, returns
  /// ActionResult::FailureTargetNotAContainer.
  /// Otherwise, calls Lua function "can_contain()" for the Entity's type.
  /// @param entity Entity to check.
  /// @return ActionResult specifying whether the entity can be held here.
  ActionResult can_contain(EntityId entity);

  /// Syntactic sugar for calling call_lua_function().
  template < typename ReturnType, typename ArgType = lua_Integer>
  ReturnType call_lua_function(std::string function_name,
                               std::vector<ArgType> const& args = {},
                               ReturnType default_result = ReturnType())
  {
    return the_lua_instance.call_thing_function<ReturnType, ArgType>(function_name, get_id(), args, default_result);
  }

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
  /// Used by find_seen_tiles.
  void do_recursive_visibility(int octant,
                               int depth = 1,
                               float slope_A = 1,
                               float slope_B = 0);

  /// Set the location of this entity.
  /// Does no checks, nor does it update the source/target inventories.
  /// Those are the responsibility of the caller.
  void set_location(EntityId target);

  /// Gets this location's maptile.
  virtual MapTile* _get_maptile() const;

  /// Process this Entity for a single tick.
  /// The function returns false to indicate to its parent that it no longer
  /// exists and should be deleted.
  /// @return true if the Entity continues to exist after the tick;
  ///         false if the Entity ceases to exist.
  virtual bool _process_self();

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
  EntityId m_ref;

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

  /// Queue of pending_actions to be performed.
  ActionQueue m_pending_actions;

  /// Map of items wielded.
  WieldingMap m_wielded_items;

  /// Map of entities worn.
  WearingMap m_equipped_items;

  /// Outline color for walls when drawing on-screen.
  static sf::Color const wall_outline_color_;
};

#endif // THING_H
