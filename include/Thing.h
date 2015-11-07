#ifndef THING_H
#define THING_H

#include "Action.h"
#include "ActionResult.h"
#include "BodyPart.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "GameObject.h"
#include "Gender.h"
#include "MapId.h"
#include "Metadata.h"

#include <memory>
#include <string>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <SFML/Graphics.hpp>

// Forward declarations
class AIStrategy;
class Entity;
class MapTile;
class Inventory;
class ThingFactory;
class ThingRef;
class Thing;

// Using Declarations

/// Typedef for the factory method.
using ThingCreator = std::shared_ptr<Thing>(*)(void);

#include "ThingImpl.h"
#include "UsesPimpl.h"

// Thing is any object in the game, animate or not.
class Thing :
  public GameObject,
  virtual public boost::noncopyable
{
  friend class boost::object_pool < Thing >;
  friend class AIStrategy;
  friend class ThingFactory;
  friend class ThingManager;

  public:
    virtual ~Thing();

    /// Queue an action for this Entity to perform.
    void queue_action(Action action);

    /// Return whether there is an action pending for this Entity.
    bool pending_action() const;

    /// Returns true if this thing is the current player.
    /// By default, returns false. Overridden by Entity class.
    virtual bool is_player() const;

    std::string const& get_type() const;
    std::string const& get_parent_type() const;

    /// Return whether a Thing is wielded by this Entity.
    /// This is used by InventoryArea to show wielded status.
    /// @param[in] thing Thing to check
    /// @return true if the Thing is wielded by the Entity.
    bool is_wielding(ThingRef thing);

    /// Return whether a Thing is wielded by this Entity.
    /// This is used by InventoryArea to show wielded status.
    /// @param[in] thing Thing to check
    /// @param[out] number Hand number it is wielded in.
    /// @return true if the Thing is wielded by the Entity.
    bool is_wielding(ThingRef thing, unsigned int& number);

    /// Return whether a Thing is equipped (worn) by this Entity.
    /// @param[in] thing Thing to check
    /// @return true if the Thing is being worn.
    bool has_equipped(ThingRef thing);

    /// Return whether a Thing is being worn by this Entity.
    /// @param[in] thing Thing to check
    /// @param[out] location of the worn Thing, if worn
    /// @return true if the Thing is being worn.
    bool has_equipped(ThingRef thing, WearLocation& location);

    /// Return whether a Thing is within reach of the Entity.
    /// @param[in] thing Thing to check
    /// @return true if the Thing is in the Entity's inventory or is at the
    ///         same location as the Entity, false otherwise.
    bool can_reach(ThingRef thing);

    /// Attempt to attack a thing.
    /// @param[in] thing Thing to attack.
    /// @param[out] action_time The time it took to attack it.
    /// @return true if attack was performed (whether it succeeded or not),
    ///         false if it wasn't performed
    bool do_attack(ThingRef thing, unsigned int& action_time);

    /// Return whether the Entity can drink the requested Thing.
    /// @param[in] thing Thing to try to drink
    /// @param[out] action_time The time it will take to drink it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_drink(ThingRef thing, unsigned int& action_time);

    /// Attempt to drink a thing.
    /// @param[in] thing Thing to try to drink
    /// @param[out] action_time The time it took to drink it.
    /// @return true if object is drank, false if not
    bool do_drink(ThingRef thing, unsigned int& action_time);

    /// Return whether the Entity can drop the requested Thing.
    /// @param[in] thing Thing to try to drop
    /// @param[out] action_time The time it will take to drop it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_drop(ThingRef thing, unsigned int& action_time);

    /// Attampt to drop a thing.
    /// @param[in] thing Thing to try to drop
    /// @param[out] action_time The time it took to drop it.
    /// @return true if object is dropped, false if not
    bool do_drop(ThingRef thing, unsigned int& action_time);

    /// Return whether the Entity can eat the requested Thing.
    /// @param[in] thing Thing to try to eat
    /// @param[out] action_time The time it will take to eat it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_eat(ThingRef thing, unsigned int& action_time);

    /// Attempt to eat a thing.
    bool do_eat(ThingRef thing, unsigned int& action_time);

    /// Return whether the Entity can mix these two Things.
    /// @param[in] thing1 First thing to mix.
    /// @param[in] thing2 Second thing to mix.
    /// @param[out] action_time The time it will take to mix.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_mix(ThingRef thing1, ThingRef thing2, unsigned int& action_time);

    /// Attempt to mix two things.
    bool do_mix(ThingRef thing1, ThingRef thing2, unsigned int& action_time);

    /// Attempt to move in a particular direction.
    /// @param[in] direction Direction to move in
    /// @param[out] action_time The time it took to move
    /// @return true on success; false if the move was prevented.
    virtual bool do_move(Direction direction, unsigned int& action_time);

    /// Return whether the Entity can put thing into container.
    /// @param[in] thing Thing to put in.
    /// @param[in] container Thing to put it into.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_put_into(ThingRef thing, ThingRef container, unsigned int& action_time);

    /// Attempt to put a thing into a container.
    bool do_put_into(ThingRef thing, ThingRef container, unsigned int& action_time);

    /// Return whether the Entity can use the requested Thing.
    /// @param[in] thing Thing to try to use
    /// @param[out] action_time The time it will take to use it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_use(ThingRef thing, unsigned int& action_time);

    /// Attempt to use a thing.
    bool do_use(ThingRef thing, unsigned int& action_time);

    template<typename T>
    T get_intrinsic(std::string key, T default_value = T()) const
    {
      return pImpl->metadata.get_intrinsic<T>(key, default_value);
    }

    template<typename T>
    T get_property(std::string key, T default_value = T())
    {      
      PropertyDictionary& properties = pImpl->properties;

      if (properties.contains(key))
      {
        return properties.get<T>(key);
      }
      else
      {
        T value = pImpl->metadata.get_default<T>(key, default_value);
        properties.set<T>(key, value);
        return value;
      }
    }

    template<typename T>
    bool set_property(std::string key, T value)
    {
      PropertyDictionary& properties = pImpl->properties;
      bool existed = properties.contains(key);
      properties.set<T>(key, value);

      return existed;
    }

    template<typename T>
    void add_to_property(std::string key, T add_value)
    {
      PropertyDictionary& properties = pImpl->properties;
      T existing_value = properties.get<T>(key);
      T new_value = existing_value + add_value;
      properties.set<T>(key, new_value);
    }

    /// Get the quantity this thing represents.
    unsigned int get_quantity();

    /// Set the quantity this thing represents.
    void set_quantity(unsigned int quantity);

    /// Return a reference to this thing.
    ThingRef get_ref() const;

    /// Return the location of this thing.
    ThingRef get_location() const;

    /// Traverse the line of sight to a map tile, setting visibility
    /// for the tiles between.
    /// @warning Assumes entity is on a map, and that the ending coordinates
    ///          are valid for the map the entity is on!
    void traverseLineOfSightTo(int xEnd, int yEnd);

    /// Return whether the Entity can see the requested Thing.
    bool can_see(ThingRef thing);

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
    std::string get_memory_at(int x, int y) const;

    /// Get the remembered tile type at the specified coordinates.
    std::string get_memory_at(sf::Vector2i coords) const;

    /// Add the memory of a particular tile to a VertexArray.
    void add_memory_vertices_to(sf::VertexArray& vertices, int x, int y);

    /// Check if the Entity can move in the specified direction.
    bool can_move(Direction direction);

    /// Return whether the Entity can pick up the requested Thing.
    /// The base method checks to make sure the Thing is at the same location
    /// as the Entity, and that the Entity's inventory can contain it.
    /// @param[in] thing Thing to try to pick up
    /// @param[out] action_time The time it will take to pick it up
    /// @return ActionResult indicating what happened.
    ActionResult can_pick_up(ThingRef thing, unsigned int& action_time);

    /// Attempt to pick a thing up.
    /// @param[in] thing Thing to try to pick up
    /// @param[out] action_time The time it took to pick up
    /// @return true if object is picked up, false if not
    bool do_pick_up(ThingRef thing, unsigned int& action_time);

    /// Return whether the Entity can take a thing out of its container.
    /// @param[in] thing Thing to take out.
    /// @return ActionResult indicating what happened.
    ActionResult can_take_out(ThingRef thing, unsigned int& action_time);

    /// Attempt to take a thing out of its container.
    bool do_take_out(ThingRef thing_id, unsigned int& action_time);

    ActionResult can_read(ThingRef thing_id, unsigned int& action_time);

    bool do_read(ThingRef thing_id, unsigned int& action_time);

    ActionResult can_throw(ThingRef thing_id, unsigned int& action_time);

    /// Attempt to toss/throw a thing in a particular direction.
    bool do_throw(ThingRef thing_id, Direction& direction, unsigned int& action_time);

    ActionResult can_deequip(ThingRef thing_id, unsigned int& action_time);

    /// Attempt to de-equip (remove) a thing.
    bool do_deequip(ThingRef thing_id, unsigned int& action_time);

    ActionResult can_equip(ThingRef thing_id, unsigned int& action_time);

    /// Attempt to equip (wear) a thing.
    bool do_equip(ThingRef thing_id, unsigned int& action_time);

    ActionResult can_wield(ThingRef thing_id, unsigned int hand, unsigned int& action_time);

    /// Attempt to wield a thing.
    /// @param[in] thing Thing to wield, or empty ptr if unwielding everything.
    /// @param[in] hand Hand to wield it in.
    /// @param[out] action_time Time it takes to wield.
    bool do_wield(ThingRef thing_id, unsigned int hand, unsigned int& action_time);

    /// Return whether this Entity can currently see.
    /// @todo Implement blindness counter, blindness due to wearing blindfold,
    ///       et cetera.
    bool can_currently_see();

    /// Return whether this Entity can currently move.
    /// @todo Implement paralysis counter, and/or other reasons to be immobile.
    bool can_currently_move();

    void set_gender(Gender gender);

    Gender get_true_gender() const;

    Gender get_gender() const;

    /// Get the number of a particular body part the Entity has.
    unsigned int get_bodypart_number(BodyPart part) const;

    /// Get the appropriate body part name for the Entity.
    std::string get_bodypart_name(BodyPart part) const;

    /// Get the appropriate body part plural for the Entity.
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
    std::string get_display_name() const;

    /// Return this object's plural.
    std::string get_display_plural() const;

    /// Get the thing's proper name (if any).
    std::string get_proper_name();

    /// Set this thing's proper name.
    void set_proper_name(std::string name);

    /// Return a string that identifies this thing.
    /// Returns "the/a/an" and a description of the thing, such as
    /// "the chair".
    /// If it is carried by the player, it'll return "your (thing)".
    /// If it IS the player, it'll return "you".
    /// Likewise, if it is carried by another Entity it'll return
    /// "(Entity)'s (thing)".
    /// @param definite   If true, uses definite articles. 
    ///                   If false, uses indefinite articles.
    ///                   Defaults to true.
    std::string get_identifying_string(bool definite = true);

    /// Return a string that identifies this thing without using possessives.
    /// The same as get_identifying_string, but without using any possessives.
    /// @param definite   If true, uses definite articles. 
    ///                   If false, uses indefinite articles.
    ///                   Defaults to true.
    std::string get_identifying_string_without_possessives(bool definite = true);

    /// Choose the proper possessive form
    /// For a Thing, this is simply "the", as Things cannot own things.
    /// This function checks to see if this Thing is currently designated as
    /// the player (in the ThingFactory).  If so, it returns "your".  If not,
    /// it returns get_name() + "'s".
    /// @note If you want a possessive pronoun like his/her/its/etc., use
    /// get_possessive_adjective().
    std::string get_possessive();


    /// Choose which verb form to use based on first/second/third person.
    /// This function checks to see if this Thing is currently designated as
    /// the player (in the ThingFactory).  If so, it returns the string passed
    /// as verb2; otherwise, it returns the string passed as verb3.
    /// @param verb2 The second person verb form, such as "shake"
    /// @param verb3 The third person verb form, such as "shakes"
    std::string const& choose_verb(std::string const& verb2,
                                   std::string const& verb3);

    /// Return this thing's mass.
    int get_mass();

    /// Get the appropriate subject pronoun for the Thing.
    std::string const& get_subject_pronoun() const;

    /// Get the appropriate object pronoun for the Thing.
    std::string const& get_object_pronoun() const;

    /// Get the appropriate reflexive pronoun for the Thing.
    std::string const& get_reflexive_pronoun() const;

    /// Get the appropriate possessive adjective for the Thing.
    std::string const& get_possessive_adjective() const;

    /// Get the appropriate possessive pronoun for the Thing.
    std::string const& get_possessive_pronoun() const;

    /// Return the coordinates of the tile representing the thing.
    sf::Vector2u get_tile_sheet_coords(int frame);

    /// Add this Thing to a VertexArray to be drawn.
    /// @param vertices Array to add vertices to.
    /// @param use_lighting If true, calculate lighting when adding.
    ///                     If false, store directly w/white bg color.
    /// @param frame Animation frame number.
    void add_vertices_to(sf::VertexArray& vertices,
                         bool use_lighting = true,
                         int frame = 0) override;

    /// Draw this Thing onto a RenderTexture, at the specified coordinates.
    /// @param target Texture to draw onto.
    /// @param target_coords Coordinates to draw the Thing at.
    /// @param target_size Target size of thing, in pixels.
    /// @param use_lighting If true, calculate lighting when adding.
    ///                     If false, store directly w/white bg color.
    /// @param frame Animation frame number.
    void draw_to(sf::RenderTexture& target,
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
    void be_lit_by(ThingRef light);

    /// Attempt to destroy this Thing.
    void destroy();

    /// Attempt to move this Thing into a location.
    bool move_into(ThingRef new_location);

    /// Return whether or not this thing can be moved from its current location by this Entity.
    /// The default behavior for this is to return true.
    bool is_movable_by(ThingRef thing);

    /// Return whether or not this thing can be activated by this Entity.
    /// The default behavior for this is to return false.
    bool is_usable_by(ThingRef thing);

    /// Return whether or not this thing can be drank by this Entity.
    /// The default behavior for this is to return false.
    bool is_drinkable_by(ThingRef thing, ThingRef contents);

    /// Return whether or not this thing can be eaten by this Entity.
    /// The default behavior for this is to return false.
    bool is_edible_by(ThingRef thing);

    /// Return whether or not this thing can be read by this Entity.
    /// The default behavior for this is to return false.
    bool is_readable_by(ThingRef thing);

    /// Return whether or not this thing can be mixed with another Thing.
    /// The default behavior for this is to return false.
    bool is_miscible_with(ThingRef thing);

    /// Return the body part this thing is equippable on.
    /// If thing is not equippable, return BodyPart::Count.
    BodyPart is_equippable_on() const;

    /// Process this Thing and its inventory for a single tick.
    bool process();

    /// Perform an action when this thing is activated.
    /// If this function returns false, the action is aborted.
    bool perform_action_activated_by(ThingRef actor);

    /// Perform an action when this thing collides with another thing.
    void perform_action_collided_with(ThingRef thing);

    /// Perform an action when this thing is eaten.
    /// If this function returns Failure, the action is aborted.
    ActionResult perform_action_drank_by(ThingRef actor, ThingRef contents);

    /// Perform an action when this thing is dropped.
    /// If this function returns false, the action is aborted.
    bool perform_action_dropped_by(ThingRef actor);

    /// Perform an action when this thing is eaten.
    /// If this function returns Failure, the action is aborted.
    ActionResult perform_action_eaten_by(ThingRef actor);

    /// Perform an action when this thing is used.
    /// If this function returns false, the action is aborted.
    bool perform_action_used_by(ThingRef actor);

    /// Perform an action when this thing is picked up.
    /// If this function returns false, the action is aborted.
    bool perform_action_picked_up_by(ThingRef actor);

    /// Perform an action when this thing is put into another thing.
    /// If this function returns false, the action is aborted.
    bool perform_action_put_into_by(ThingRef container, ThingRef actor);

    /// Perform an action when this thing is taken out its container.
    /// If this function returns false, the action is aborted.
    bool perform_action_taken_out_by(ThingRef actor);

    /// Perform an action when this thing is read.
    /// If this function returns Failure, the action is aborted.
    ActionResult perform_action_read_by(ThingRef actor);

    /// Perform an action when this thing hits an entity.
    /// This action executes when the thing is wielded by an entity, and an
    /// attack successfully hits its target.  It is a side-effect in addition
    /// to the damage done by Entity::attack(entity).
    /// @see Entity::attack
    void perform_action_attack_hits(ThingRef target);

    /// Perform an action when this thing is thrown.
    /// If this function returns false, the action is aborted.
    bool perform_action_thrown_by(ThingRef actor, Direction direction);

    /// Perform an action when this thing is de-equipped (taken off).
    /// If this function returns false, the action is aborted.
    bool perform_action_deequipped_by(ThingRef actor, WearLocation& location);

    /// Perform an action when this thing is equipped.
    /// If this function returns false, the action is aborted.
    bool perform_action_equipped_by(ThingRef actor, WearLocation& location);

    /// Perform an action when this thing is unwielded.
    /// If this function returns false, the action is aborted.
    bool perform_action_unwielded_by(ThingRef actor);

    /// Perform an action when this thing is wielded.
    /// If this function returns false, the action is aborted.
    bool perform_action_wielded_by(ThingRef actor);

    /// Perform an action when this thing is fired.
    /// If this function returns false, the action is aborted.
    bool perform_action_fired_by(ThingRef actor, Direction direction);

    /// Returns whether the Thing can merge with another Thing.
    /// Calls an overridden subclass function.
    bool can_merge_with(ThingRef other) const;

    /// Returns whether the Thing can hold a certain thing.
    /// If Thing's inventory size is 0, returns
    /// ActionResult::FailureTargetNotAContainer.
    /// Otherwise, calls Lua function "can_contain()" for the Thing's type.
    /// @param thing Thing to check.
    /// @return ActionResult specifying whether the thing can be held here.
    ActionResult can_contain(ThingRef thing);

  private:
    /// Pimpl implementation
    /// We don't use CopyablePimpl because you can't copy a Thing;
    /// you can clone it but that's a different concept.
    Pimpl<ThingImpl> pImpl;

    /// Named Constructor
    Thing(Metadata& metadata, ThingRef ref);

    /// Floor Constructor
    Thing(MapTile* map_tile, Metadata& metadata, ThingRef ref);

    /// Clone Constructor
    Thing(Thing const& original, ThingRef ref);

    /// Get a reference to this Entity's map memory.
    std::vector<std::string>& get_map_memory();

    /// Perform the recursive visibility scan for an octant.
    /// Used by find_seen_tiles.
    void do_recursive_visibility(int octant,
      int depth = 1,
      double slope_A = 1,
      double slope_B = 0);

    /// Set the location of this thing.
    /// Does no checks, nor does it update the source/target inventories.
    /// Those are the responsibility of the caller.
    void set_location(ThingRef target);

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

    // Static Lua functions.
    // @todo (Maybe these should be part of ThingManager instead?)

    /// Lua function to create a new Thing.
    /// Takes two parameters:
    ///   - ID of the Thing it will be stored in
    ///   - Type of the new Thing
    /// It returns:
    ///   - ID of the newly created Thing, or nil if it could not be created.
    static int LUA_create(lua_State* L);

    /// Lua function to get the ID of the player.
    /// Takes no parameters.
    /// It returns:
    ///   - ID of the player character.
    static int LUA_get_player(lua_State* L);


    /// Lua function to get the coordinates of a Thing.
    /// Takes one parameter:
    ///   - ID of the Thing to get coordinates of.
    /// It returns:
    ///   - (x, y) coordinates of the Thing, or nil if not on the player's Map.
    static int LUA_get_coords(lua_State* L);

    static int LUA_get_type(lua_State* L);
    static int LUA_get_display_name(lua_State* L);
    static int LUA_get_display_plural(lua_State* L);
    static int LUA_get_parent_type(lua_State* L);
    static int LUA_get_intrinsic_flag(lua_State* L);
    static int LUA_get_intrinsic_value(lua_State* L);
    static int LUA_get_intrinsic_string(lua_State* L);
    static int LUA_get_property_flag(lua_State* L);
    static int LUA_get_property_value(lua_State* L);
    static int LUA_get_property_string(lua_State* L);
    static int LUA_set_property_flag(lua_State* L);
    static int LUA_set_property_value(lua_State* L);
    static int LUA_set_property_string(lua_State* L);

    /// Syntactic sugar for calling Metadata::call_lua_function.
    ActionResult call_lua_function(std::string function_name,
      std::vector<lua_Integer> const& args,
      ActionResult default_result = ActionResult::Success);

    /// Syntactic sugar for calling Metadata::call_lua_function_bool.
    bool call_lua_function_bool(std::string function_name,
      std::vector<lua_Integer> const& args,
      bool default_result = true);

    /// Syntactic sugar for calling Metadata::call_lua_function_v2u.
    sf::Vector2u call_lua_function_v2u(std::string function_name,
      std::vector<lua_Integer> const& args,
      sf::Vector2u default_result = sf::Vector2u(0, 0));

};

#endif // THING_H
