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
#include "Metadata.h"

// Forward declarations
class AIStrategy;
class Entity;
class MapTile;
class Inventory;
class ThingFactory;
class ThingId;
class Thing;

// Using Declarations

/// Typedef for the factory method.
using ThingCreator = std::shared_ptr<Thing>(*)(void);

#include "ThingImpl.h"
#include "UsesPimpl.h"

// Thing is any object in the game, animate or not.
class Thing
  :
  public GameObject
{
  friend class AIStrategy;
  friend class ThingFactory;
  friend class ThingManager;

public:
  virtual ~Thing();

  /// Queue an action for this Entity to perform.
  /// pAction The Action to queue. The Action is MOVED when queued,
  ///         e.g. pAction will be `nullptr` after queuing.
  void queue_action(std::unique_ptr<Action> pAction);

  /// Return whether there is an action pending for this Entity.
  bool action_is_pending() const;

  /// Return whether there is an action currently in progress for this Entity.
  bool action_is_in_progress();

  /// Get the thing being wielded in the specified hand, if any.
  ThingId get_wielding(unsigned int & hand);

  /// Returns true if this thing is the current player.
  /// By default, returns false. Overridden by Entity class.
  virtual bool is_player() const;

  StringKey const& get_type() const;
  StringKey const& get_parent_type() const;

  bool is_subtype_of(StringKey that_type) const;

  /// Return whether a Thing is wielded by this Entity.
  /// This is used by InventoryArea to show wielded status.
  /// @param[in] thing Thing to check
  /// @return true if the Thing is wielded by the Entity.
  bool is_wielding(ThingId thing);

  /// Return whether a Thing is wielded by this Entity.
  /// This is used by InventoryArea to show wielded status.
  /// @param[in] thing Thing to check
  /// @param[out] number Hand number it is wielded in.
  /// @return true if the Thing is wielded by the Entity.
  bool is_wielding(ThingId thing, unsigned int& number);

  /// Return whether a Thing is equipped (worn) by this Entity.
  /// @param[in] thing Thing to check
  /// @return true if the Thing is being worn.
  bool has_equipped(ThingId thing);

  /// Return whether a Thing is being worn by this Entity.
  /// @param[in] thing Thing to check
  /// @param[out] location of the worn Thing, if worn
  /// @return true if the Thing is being worn.
  bool has_equipped(ThingId thing, WearLocation& location);

  /// Return whether a Thing is within reach of the Entity.
  /// @param[in] thing Thing to check
  /// @return true if the Thing is in the Entity's inventory or is at the
  ///         same location as the Entity, false otherwise.
  bool can_reach(ThingId thing);

  /// Return whether a Thing is adjacent to this Entity.
  /// @param[in] thing Thing to check
  /// @return true if the Thing is at the same place or adjacent to this Entity, false otherwise.
  bool is_adjacent_to(ThingId thing);

  /// Die.
  /// @return True if the Thing died, false if the death was avoided.
  bool do_die();

  /// Attempt to (melee) attack a thing.
  /// @param[in] thing Thing to attack.
  /// @param[out] action_time The time it took to attack it.
  /// @return true if attack was performed (whether it succeeded or not),
  ///         false if it wasn't performed
  bool do_attack(ThingId thing, unsigned int& action_time);

  /// Get an intrinsic of this Thing.
  /// If the intrinsic is not found, returns the default value.
  /// @param key            Name of the intrinsic to get.
  /// @param default_value  Default value to use, if any.
  /// @return The intrinsic (or default) value for that key.
  template<typename T>
  T get_intrinsic(StringKey key, T default_value = T()) const
  {
    return pImpl->metadata.get_intrinsic<T>(key, default_value);
  }

  /// Get a base property of this Thing.
  /// If the base property is not found, the method falls back upon the
  /// intrinsic for that property (if any).
  /// @param key            Name of the property to get.
  /// @param default_value  Default value to use, if any.
  /// @return The property (or default) value for that key.
  template<typename T>
  T get_base_property(StringKey key, T default_value = T())
  {
    ModifiablePropertyDictionary& properties = pImpl->properties;

    if (properties.contains(key))
    {
      return properties.get<T>(key);
    }
    else
    {
      T value = pImpl->metadata.get_intrinsic<T>(key, default_value);
      properties.set<T>(key, value);
      return value;
    }
  }

  /// Sets a base property of this Thing.
  /// If the base property is not found, it is created.
  ///
  /// @param key    Key of the property to set.
  /// @param value  Value to set the property to.
  /// @return Boolean indicating whether the property previously existed.
  template<typename T>
  bool set_base_property(StringKey key, T value)
  {
    ModifiablePropertyDictionary& properties = pImpl->properties;
    bool existed = properties.contains(key);
    properties.set<T>(key, value);

    return existed;
  }

  /// Adds to a base property of this Thing.
  /// If the base property is not found, it is created.
  /// @param key    Key of the property to set.
  /// @param value  Value to add to the property.
  template<typename T>
  void add_to_base_property(StringKey key, T add_value)
  {
    ModifiablePropertyDictionary& properties = pImpl->properties;
    T existing_value = properties.get<T>(key);
    T new_value = existing_value + add_value;
    properties.set<T>(key, new_value);
  }

  /// Get a modified property of this Thing.
  /// If the modified property is not found, the method falls back upon the
  /// base value for that property (if any).
  /// @param key            Name of the property to get.
  /// @param default_value  Default value to use, if any.
  /// @return The modified (or base) property value for that key.
  template<typename T>
  T get_modified_property(StringKey key, T default_value = T())
  {
    ModifiablePropertyDictionary& properties = pImpl->properties;

    if (!properties.contains(key))
    {
      T value = pImpl->metadata.get_intrinsic<T>(key, default_value);
      properties.set<T>(key, value);
    }

    return properties.get_modified<T>(key);
  }

  /// Add a property modifier to this Thing.
  /// @param  key               Name of property to modify.
  /// @param  id                ID of Thing that is responsible for modifying it.
  /// @param  expiration_ticks  Number of ticks until modifier is removed, or 0
  ///                           if it is never removed.
  ///
  /// @see ModifiablePropertyDictionary::add_modifier
  ///
  /// @return True if the function was added; false if it already existed.
  bool add_modifier(StringKey key, ThingId id, unsigned int expiration_ticks = 0);

  /// Remove all modifier functions for a given key and thing ID.
  /// @param  key               Name of property to modify.
  /// @param  id                ID of Thing that is responsible for modifying it.
  ///
  /// @return The number of modifiers erased.
  unsigned int remove_modifier(StringKey key, ThingId id);

  /// Get the quantity this thing represents.
  unsigned int get_quantity();

  /// Set the quantity this thing represents.
  void set_quantity(unsigned int quantity);

  /// Return a reference to this thing.
  ThingId get_id() const;

  /// Return the root location of this Thing, traversing up the location
  /// chain until MU is reached.
  ThingId get_root_location() const;

  /// Return the location of this thing.
  ThingId get_location() const;

  /// Return whether the Entity can see the requested Thing.
  bool can_see(ThingId thing);

  /// Return whether the Entity can see the requested tile.
  bool can_see(sf::Vector2i coords);

  /// Return whether the Entity can see the requested tile.
  bool can_see(int x, int y);

  /// Find out which tiles on the map can be seen by this Entity.
  /// In the process, tiles in the Entity's visual memory are updated.
  /// This method uses a recursive raycasting algorithm to figure out what
  /// can be seen at a particular position.
  void find_seen_tiles();

  /// Get the remembered tile type at the specified coordinates.
  StringKey get_memory_at(int x, int y) const;

  /// Get the remembered tile type at the specified coordinates.
  StringKey get_memory_at(sf::Vector2i coords) const;

  /// Add the memory of a particular tile to a VertexArray.
  void add_memory_vertices_to(sf::VertexArray& vertices, int x, int y);

  ActionResult can_deequip(ThingId thing_id, unsigned int& action_time);

  /// Attempt to de-equip (remove) a thing.
  bool do_deequip(ThingId thing_id, unsigned int& action_time);

  ActionResult can_equip(ThingId thing_id, unsigned int& action_time);

  /// Attempt to equip (wear) a thing.
  bool do_equip(ThingId thing_id, unsigned int& action_time);

  ActionResult can_wield(ThingId thing_id, unsigned int hand, unsigned int& action_time);

  void set_wielded(ThingId thing, unsigned int hand);

  void set_worn(ThingId thing, WearLocation location);

  /// Return whether this Entity can currently see.
  /// @todo Implement blindness counter, blindness due to wearing blindfold,
  ///       et cetera.
  bool can_currently_see();

  /// Return whether this Entity can currently move.
  /// @todo Implement paralysis counter, and/or other reasons to be immobile.
  bool can_currently_move();

  void set_gender(Gender gender);

  Gender get_gender() const;

  Gender get_gender_or_you() const;

  /// Get the number of a particular body part the Entity has.
  unsigned int get_bodypart_number(BodyPart part) const;

  /// Get the appropriate body part name for the Entity.
  StringDisplay get_bodypart_name(BodyPart part) const;

  /// Get the appropriate body part plural for the Entity.
  StringDisplay get_bodypart_plural(BodyPart part) const;

  /// Get the appropriate description for a body part.
  /// This takes the body part name and the number referencing the particular
  /// part and comes up with a description.
  /// For example, for most creatures with two hands, hand #0 will be the
  /// "right hand" and hand #1 will be the "left hand".
  /// In most cases the default implementation here will work, but if a
  /// creature has (for example) a strange configuration of limbs this can be
  /// overridden.
  StringDisplay get_bodypart_description(BodyPart part, unsigned int number);

  /// Returns true if a particular Action can be performed on this Thing by
  /// the specified Thing.
  /// @param thing  The Thing doing the Action.
  /// @param action Reference to the Action to perform.
  /// @return true if the Action can be performed, false otherwise.
  bool can_have_action_done_by(ThingId thing, Action& action);

  /// Returns a reference to the inventory.
  Inventory& get_inventory();

  /// Returns true if this thing is inside another Thing.
  bool is_inside_another_thing() const;

  /// Get the MapTile this thing is on, or nullptr if not on a map.
  MapTile* get_maptile() const;

  /// Return the MapId this Thing is currently on, or 0 if not on a map.
  MapId get_map_id() const;

  /// Return this thing's description.
  /// Adds adjective qualifiers (such as "fireproof", "waterproof", etc.)
  /// @todo Add adjective qualifiers.s
  StringDisplay get_display_name() const;

  /// Return this object's plural.
  StringDisplay get_display_plural() const;

  /// Get the thing's proper name (if any).
  StringDisplay get_proper_name();

  /// Set this thing's proper name.
  void set_proper_name(StringDisplay name);

  /// Return a string that identifies this thing.
  /// If it IS the player, it'll return "you".
  /// Otherwise it calls get_identifying_string().
  ///
  /// @param definite   If true, uses definite articles.
  ///                   If false, uses indefinite articles.
  ///                   Defaults to true.
  StringDisplay get_you_or_identifying_string(bool definite = true);

  /// Return a string that identifies this thing.
  /// If it matches the object passed in as "other", it'll return
  /// the appropriate reflexive pronoun ("yourself", "itself", etc.).
  /// Otherwise it calls get_identifying_string().
  ///
  /// @param other      The "other" to compare to.
  /// @param definite   If true, uses definite articles.
  ///                   If false, uses indefinite articles.
  ///                   Defaults to true.
  StringDisplay get_self_or_identifying_string(ThingId other, bool definite = true);

  /// Return a string that identifies this thing.
  /// Returns "the/a/an" and a description of the thing, such as
  /// "the chair".
  /// If it is carried by the player, it'll return "your (thing)".
  /// Likewise, if it is carried by another Entity it'll return
  /// "(Entity)'s (thing)".
  /// @param definite   If true, uses definite articles.
  ///                   If false, uses indefinite articles.
  ///                   Defaults to true.
  StringDisplay get_identifying_string(bool definite = true);

  /// Return a string that identifies this thing without using possessives.
  /// The same as get_identifying_string, but without using any possessives.
  /// @param definite   If true, uses definite articles.
  ///                   If false, uses indefinite articles.
  ///                   Defaults to true.
  StringDisplay get_identifying_string_without_possessives(bool definite = true);

  /// Choose the proper possessive form
  /// For a Thing, this is simply "the", as Things cannot own things.
  /// This function checks to see if this Thing is currently designated as
  /// the player (in the ThingFactory).  If so, it returns "your".  If not,
  /// it returns get_name() + "'s".
  /// @note If you want a possessive pronoun like his/her/its/etc., use
  /// get_possessive_adjective().
  StringDisplay get_possessive();

  /// Choose which verb form to use based on first/second/third person.
  /// This function checks to see if this Thing is currently designated as
  /// the player (in the ThingFactory), or has a quantity greater than zero.
  /// If so, it returns the string passed as verb2; otherwise, it returns the
  /// string passed as verb3.
  /// @param verb2 The second person or plural verb form, such as "shake"
  /// @param verb3 The third person verb form, such as "shakes"
  StringDisplay const& choose_verb(StringDisplay const& verb2,
                                   StringDisplay const& verb3);

  /// Return this thing's mass.
  int get_mass();

  /// Get the appropriate subject pronoun for the Thing.
  StringDisplay const& get_subject_pronoun() const;

  /// Get the appropriate object pronoun for the Thing.
  StringDisplay const& get_object_pronoun() const;

  /// Get the appropriate reflexive pronoun for the Thing.
  StringDisplay const& get_idlexive_pronoun() const;

  /// Get the appropriate possessive adjective for the Thing.
  StringDisplay const& get_possessive_adjective() const;

  /// Get the appropriate possessive pronoun for the Thing.
  StringDisplay const& get_possessive_pronoun() const;

  /// Return the coordinates of the tile representing the thing.
  sf::Vector2u get_tile_sheet_coords(int frame);

  /// Add this Thing to a VertexArray to be drawn.
  /// @param vertices Array to add vertices to.
  /// @param use_lighting If true, calculate lighting when adding.
  ///                     If false, store directly w/white bg color.
  /// @param frame Animation frame number.
  void add_floor_vertices_to(sf::VertexArray& vertices,
                             bool use_lighting = true,
                             int frame = 0) override;

  /// Draw this Thing onto a RenderTexture, at the specified coordinates.
  /// @param target Texture to draw onto.
  /// @param target_coords Coordinates to draw the Thing at.
  /// @param target_size Target size of thing, in pixels.
  /// @param use_lighting If true, calculate lighting when adding.
  ///                     If false, store directly w/white bg color.
  /// @param frame Animation frame number.
  void draw_to(sf::RenderTarget& target,
               sf::Vector2f target_coords,
               unsigned int target_size = 0,
               bool use_lighting = true,
               int frame = 0);

  /// Simple check to see if a Thing is opaque.
  bool is_opaque() const;

  /// Provide light to this Thing's surroundings.
  /// If Thing is not opaque, calls light_up_surroundings() for each Thing
  /// in its inventory.
  void light_up_surroundings();

  /// Receive light from the specified light source.
  /// The default behavior is to pass the light source to the location if
  /// this Thing is opaque.
  void be_lit_by(ThingId light);

  /// Spill the contents of this Thing out into the location of the Thing.
  void spill();

  /// Attempt to destroy this Thing.
  void destroy();

  /// Attempt to move this Thing into a location.
  bool move_into(ThingId new_location);

  /// Return whether or not this thing can be mixed with another Thing.
  /// The default behavior for this is to return false.
  bool is_miscible_with(ThingId thing);

  /// Return the body part this thing is equippable on.
  /// If thing is not equippable, return BodyPart::Count.
  BodyPart is_equippable_on() const;

  /// Process this Thing and its inventory for a single tick.
  bool process();

  /// Perform an action when this thing dies.
  /// @return If this function returns Failure, the death is avoided.
  /// @warning The function must reset whatever caused the death in the
  ///          first place, or the Thing will just immediately die again
  ///          on the next call to process()!
  ActionResult perform_action_died();

  /// Perform an action when this thing collides with another thing.
  void perform_action_collided_with(ThingId thing);

  /// Perform an action when this thing collides with a wall.
  void perform_action_collided_with_wall(Direction d, StringKey tile_type);

  /// Perform the effects of being a object of a particular action.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @return Result of the action. If Failure, the action is aborted
  ///         (if possible).
  ActionResult be_object_of(Action & action, ThingId subject);

  /// Perform the effects of being a object of an action with a target.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @param target   The target of the action.
  /// @return Result of the action. If Failure, the action is aborted
  ///         (if possible).
  ActionResult be_object_of(Action & action, ThingId subject, ThingId target);

  /// Perform the effects of being a object of an action with a direction.
  /// @param action     The action to be the target of.
  /// @param subject    The subject performing the action.
  /// @param direction  The direction of the action.
  /// @return Result of the action. If Failure, the action is aborted
  ///         (if possible).
  ActionResult be_object_of(Action & action, ThingId subject, Direction direction);

  /// Perform an action when this thing is hit by an attack.
  ActionResult perform_action_hurt_by(ThingId subject);

  /// Perform an action when this thing is used to hit a target.
  /// This action executes when the thing is wielded by an entity, and an
  /// attack successfully hits its target.  It is a side-effect in addition
  /// to the damage done by Entity::attack(entity).
  /// @see Entity::attack
  /// @note Is there a better name for this? Current name sounds like the
  ///       object is the target, instead of the implement.
  ActionResult perform_action_attacked_by(ThingId subject, ThingId target);

  /// Perform an action when this thing is de-equipped (taken off).
  /// If this function returns false, the action is aborted.
  bool perform_action_deequipped_by(ThingId actor, WearLocation& location);

  /// Perform an action when this thing is equipped.
  /// If this function returns false, the action is aborted.
  bool perform_action_equipped_by(ThingId actor, WearLocation& location);

  /// Perform an action when this thing is unwielded.
  /// If this function returns false, the action is aborted.
  bool perform_action_unwielded_by(ThingId actor);

  /// Returns whether the Thing can merge with another Thing.
  /// Calls an overridden subclass function.
  bool can_merge_with(ThingId other) const;

  /// Returns whether the Thing can hold a certain thing.
  /// If Thing's inventory size is 0, returns
  /// ActionResult::FailureTargetNotAContainer.
  /// Otherwise, calls Lua function "can_contain()" for the Thing's type.
  /// @param thing Thing to check.
  /// @return ActionResult specifying whether the thing can be held here.
  ActionResult can_contain(ThingId thing);

private:
  /// Pimpl implementation
  /// We don't use CopyablePimpl because you can't copy a Thing;
  /// you can clone it but that's a different concept.
  Pimpl<ThingImpl> pImpl;

  /// Named Constructor
  Thing(Metadata& metadata, ThingId ref);

  /// Floor Constructor
  Thing(MapTile* map_tile, Metadata& metadata, ThingId ref);

  /// Clone Constructor
  Thing(Thing const& original, ThingId ref);

  /// Initializer; called by all constructors.
  void initialize();

  /// Get a reference to this Entity's map memory.
  std::vector<StringKey>& get_map_memory();

  /// Perform the recursive visibility scan for an octant.
  /// Used by find_seen_tiles.
  void do_recursive_visibility(int octant,
                               int depth = 1,
                               float slope_A = 1,
                               float slope_B = 0);

  /// Set the location of this thing.
  /// Does no checks, nor does it update the source/target inventories.
  /// Those are the responsibility of the caller.
  void set_location(ThingId target);

  /// Outline color for walls when drawing on-screen.
  static sf::Color const wall_outline_color_;

  /// Gets this location's maptile.
  virtual MapTile* _get_maptile() const;

  /// Process this Thing for a single tick.
  /// The function returns false to indicate to its parent that it no longer
  /// exists and should be deleted.
  /// @return true if the Thing continues to exist after the tick;
  ///         false if the Thing ceases to exist.
  virtual bool _process_self();

  /// Syntactic sugar for calling call_lua_function().
  template < typename ReturnType, typename ArgType = lua_Integer>
  ReturnType call_lua_function(std::string function_name,
                               std::vector<ArgType> const& args = {},
                               ReturnType default_result = ReturnType())
  {
    return the_lua_instance.call_thing_function<ReturnType, ArgType>(function_name, get_id(), args, default_result);
  }
};

#endif // THING_H
