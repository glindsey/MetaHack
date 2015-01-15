#ifndef THING_H
#define THING_H

#include "ActionResult.h"
#include "BodyPart.h"
#include "Direction.h"
#include "GameObject.h"
#include "MapId.h"

#include <memory>
#include <string>
#include <boost/strong_typedef.hpp>
#include <SFML/Graphics.hpp>

// Forward declarations
class Entity;
class MapTile;
class Inventory;
class LightSource;
class ThingFactory;

// Strong typedef for ThingIDs.
BOOST_STRONG_TYPEDEF(unsigned int, ThingID)


// Typedef for the factory method.
class Thing;
using ThingCreator = std::shared_ptr<Thing>(*)(void);

// Thing is any object in the game, animate or not.
class Thing :
  public GameObject,
  public std::enable_shared_from_this<Thing>
{
  friend class ThingFactory;

  /// Qualities that are duplicated when an item is copied.  Also used to
  /// compare two things when combining Aggregates.
  struct Qualities
  {
    int physical_mass;                      ///< Mass of the object
    bool open;
    bool locked;
    bool autobinds;                         ///< True = magically binds on equip
    bool bound;
    bool invisible;
  };

  public:
    virtual ~Thing();

    /// Copy this Thing and return a shared pointer to it.
    virtual std::shared_ptr<Thing> clone();

    /// Returns true if this thing is the current player.
    /// By default, returns false. Overridden by Entity class.
    virtual bool is_player() const;

    /// Get the quantity this thing represents.
    virtual unsigned int get_quantity() const;

    /// Set the quantity this thing represents.
    void set_quantity(unsigned int quantity);

    /// Return the location of this thing.
    std::shared_ptr<Thing> get_location() const;

    /// Returns a reference to the inventory.
    Inventory& get_inventory();

    /// Returns the size of this Thing's inventory.
    /// @return The inventory size, or a special value.
    /// @retval 0 This Location cannot hold other things (but still remains
    ///           a Location).
    /// @retval -1 This Location has an infinite inventory size.
    int const get_inventory_size() const;

    /// Sets the size of this Thing's inventory.
    /// @param number Size of the inventory, or a special value.
    /// @see get_inventory_size
    void set_inventory_size(int number);

    /// Returns true if this thing is inside another Thing.
    bool is_inside_another_thing() const;

    /// Get the MapTile this thing is on, or nullptr if not on a map.
    MapTile* get_maptile() const;

    /// Return the MapId this Thing is currently on, or 0 if not on a map.
    MapId get_map_id() const;

    /// Set the direction the thing is facing.
    void set_facing_direction(Direction d);

    /// Get the direction the thing is facing.
    Direction get_facing_direction() const;

    /// Set this thing's mass.
    void set_single_mass(int mass);

    /// Return this thing's mass.
    int get_single_mass() const;

    /// Set whether this item is magically bound.
    void set_bound(bool bound);

    /// Set whether this item autobinds.
    void set_autobinds(bool autobinds);

    /// Get whether this item is magically bound.
    bool is_bound() const;

    /// Get whether this item autolocks.
    bool get_autobinds() const;

    /// Returns true if this thing can be opened/closed.
    /// By default, returns false. Derived classes can override this.
    virtual bool is_openable() const;

    /// Returns true if this thing can be locked/unlocked.
    virtual bool is_lockable() const;

    /// Get whether this thing is opened.
    /// If is_openable() is false, this function always returns true.
    /// @return True if the thing is open, false otherwise.
    bool is_open() const;

    /// Set whether this thing is open.
    /// If is_openable() is false, this function does nothing.
    /// @return True if the thing is open, false otherwise.
    bool set_open(bool open);

    /// Get whether this thing is locked.
    /// If is_openable() or is_lockable() is false, this function always
    /// returns false.
    /// @return True if the thing is locked, false otherwise.
    bool is_locked() const;

    /// Set whether this thing is locked.
    /// If is_openable() or is_lockable() is false, this function does nothing.
    /// @return True if the thing is locked, false otherwise.
    bool set_locked(bool open);

    /// Get a const pointer to the Qualities struct.
    /// This is used by Aggregates to compare when combining objects.
    Qualities const* get_qualities_pointer() const;

    /// Compare this Thing's qualities with another one, and return true if
    /// they are identical.
    bool has_same_qualities_as(Thing const& other) const;

    /// Return this thing's description.
    /// Calls the overridden _get_description() and adds adjective qualifiers
    /// (such as "fireproof", "waterproof", etc.)
    virtual std::string get_description() const override final;

    /// Get the thing's proper name (if any).
    std::string get_proper_name() const;

    /// Set this thing's proper name.
    void set_proper_name(std::string name);

    /// Return a string that identifies this thing.
    /// By default, returns "the" and a description of the thing, such as
    /// "the chair".
    /// If it is carried by the player, it'll return "your (thing)".
    /// If it IS the player, it'll return "you".
    /// Likewise, if it is carried by another Entity it'll return
    /// "(Entity)'s (thing)".
    virtual std::string get_name() const;

    /// Return a string that identifies this thing.
    /// By default, returns "the" and a description of the thing, such as
    /// "the chair" or "the orange".
    /// If it IS the player, it'll return "you".
    /// Unlike get_name, get_def_name does NOT check for possession.
    virtual std::string get_def_name() const;

    /// Return a string that identifies this thing.
    /// By default, returns "a" or "an" and a description of the thing, such as
    /// "a chair" or "an orange".
    /// If it IS the player, it'll return "you".
    /// Unlike get_name, get_indef_name does NOT check for possession.
    virtual std::string get_indef_name() const;

    /// Choose the proper possessive form
    /// For a Thing, this is simply "the", as Things cannot own things.
    /// This function checks to see if this Thing is currently designated as
    /// the player (in the ThingFactory).  If so, it returns "your".  If not,
    /// it returns get_name() + "'s".
    /// @note If you want a possessive pronoun like his/her/its/etc., use
    /// get_possessive_adjective().
    virtual std::string get_possessive() const;


    /// Choose which verb form to use based on first/second/third person.
    /// This function checks to see if this Thing is currently designated as
    /// the player (in the ThingFactory).  If so, it returns the string passed
    /// as verb2; otherwise, it returns the string passed as verb3.
    /// @param verb2 The second person verb form, such as "shake"
    /// @param verb3 The third person verb form, such as "shakes"
    std::string const& choose_verb(std::string const& verb2,
                                   std::string const& verb3) const;

    /// Return this thing's mass.
    virtual int get_mass() const;

    /// Return this object's plural.
    /// By default, returns _get_description() plus "s", but this can be
    /// overridden by child classes (for example "djinn" -> "djinni").
    virtual std::string get_plural() const;

    /// Get the appropriate subject pronoun for the Thing.
    virtual std::string const& get_subject_pronoun();

    /// Get the appropriate object pronoun for the Thing.
    virtual std::string const& get_object_pronoun();

    /// Get the appropriate reflexive pronoun for the Thing.
    virtual std::string const& get_reflexive_pronoun();

    /// Get the appropriate possessive adjective for the Thing.
    virtual std::string const& get_possessive_adjective();

    /// Get the appropriate possessive pronoun for the Thing.
    virtual std::string const& get_possessive_pronoun();

    /// Return the coordinates of the tile representing the thing.
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const;

    /// Add this Thing to a VertexArray to be drawn.
    /// @param vertices Array to add vertices to.
    /// @param use_lighting If true, calculate lighting when adding.
    ///                     If false, store directly w/white bg color.
    /// @param frame Animation frame number.
    virtual void add_vertices_to(sf::VertexArray& vertices,
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
    virtual bool is_opaque() const;

    /// Provide light to this Thing's surroundings.
    /// If Thing is not opaque, calls light_up_surroundings() for each Thing
    /// in its inventory.
    virtual void light_up_surroundings();

    /// Receive light from the specified LightSource.
    /// The default behavior is to pass the light source to the location if
    /// this Thing is opaque.
    virtual void be_lit_by(LightSource& light);

    /// Attempt to destroy this Thing.
    virtual void destroy();

    /// Attempt to move this Thing into a location.
    virtual bool move_into(std::shared_ptr<Thing> new_location);

    /// Return whether or not this thing can move from its current location.
    /// The default behavior for this is to return true.
    virtual bool is_movable() const;

    /// Return whether or not this thing can be activated by this Entity.
    /// The default behavior for this is to return false.
    virtual bool usable_by(Entity const& entity) const;

    /// Return whether or not this thing can be drank by this Entity.
    /// The default behavior for this is to return false.
    virtual bool drinkable_by(Entity const& entity) const;

    /// Return whether or not this thing can be eaten by this Entity.
    /// The default behavior for this is to return false.
    virtual bool edible_by(Entity const& entity) const;

    /// Return whether or not this thing can be read by this Entity.
    /// The default behavior for this is to return false.
    virtual bool readable_by(Entity const& entity) const;

    /// Return whether or not this thing can be mixed with another Thing.
    /// The default behavior for this is to return false.
    virtual bool miscible_with(Thing const& thing) const;

    /// Return the body part this thing is equippable on.
    /// If thing is not equippable, return BodyPart::Count.
    virtual BodyPart equippable_on() const;

    /// Process this Thing and its inventory for a single tick.
    bool do_process();

    /// Perform an action when this thing is activated.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    bool perform_action_activated_by(Entity& entity);

    /// Perform an action when this thing collides with another thing.
    void perform_action_collided_with(Thing& thing);

    /// Perform an action when this thing is eaten.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    bool perform_action_drank_by(Entity& entity);

    /// Perform an action when this thing is dropped.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_dropped_by(Entity& entity);

    /// Perform an action when this thing is eaten.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    bool perform_action_eaten_by(Entity& entity);

    /// Perform an action when this thing is picked up.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_picked_up_by(Entity& entity);

    /// Perform an action when this thing is put into another thing.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_put_into(Thing& container);

    /// Perform an action when this thing is taken out its container.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_take_out();

    /// Perform an action when this thing is read.
    /// If this function returns Failure, the action is aborted.
    /// The default behavior is to do nothing and return Failure.
    ActionResult perform_action_read_by(Entity& entity);

    /// Perform an action when this thing hits an entity.
    /// This action executes when the thing is wielded by an entity, and an
    /// attack successfully hits its target.  It is a side-effect in addition
    /// to the damage done by Entity::attack(entity).
    /// The default behavior is to do nothing.
    /// @see Entity::attack
    void perform_action_attack_hits(Entity& entity);

    /// Perform an action when this thing is thrown.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_thrown_by(Entity& entity, Direction direction);

    /// Perform an action when this thing is de-equipped (taken off).
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_deequipped_by(Entity& thing, WearLocation& location);

    /// Perform an action when this thing is equipped.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    bool perform_action_equipped_by(Entity& entity, WearLocation& location);

    /// Perform an action when this thing is unwielded.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_unwielded_by(Entity& entity);

    /// Perform an action when this thing is wielded.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    bool perform_action_wielded_by(Entity& entity);

    /// Perform an action when this thing is fired.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    bool perform_action_fired_by(Entity& entity, Direction direction);

    /// Returns whether the Thing can merge with another Thing.
    /// Calls an overridden subclass function.
    bool can_merge_with(Thing const& thing) const;

    /// Returns whether the Thing can hold a certain thing.
    /// If Thing's inventory size is 0, returns
    /// ActionResult::FailureTargetNotAContainer.
    /// Otherwise, calls private virtual method _can_contain().
    /// @param thing Thing to check.
    /// @return ActionResult specifying whether the thing can be held here.
    ActionResult can_contain(Thing& thing) const;

    /// Return the type of this thing.
    //virtual char const* get_thing_type() const final;

    /// Returns whether this Thing can hold a liquid.
    virtual bool is_liquid_carrier() const;

    /// Returns whether this Thing is flammable.
    virtual bool is_flammable() const;

    /// Returns whether this Thing is corrodible.
    virtual bool is_corrodible() const;

    /// Returns whether this Thing is shatterable.
    virtual bool is_shatterable() const;

  protected:
    /// Constructor
    Thing(int inventory_size);

    /// Copy Constructor
    Thing(const Thing& original);

    struct Impl;
    std::unique_ptr<Impl> impl;

    /// Set the location of this thing.
    /// Does no checks, nor does it update the source/target inventories.
    /// Those are the responsibility of the caller.
    void set_location(std::weak_ptr<Thing> target);

    /// Static method initializing font sizes.
    static void initialize_font_sizes();

    /// Does the actual call to light surroundings.
    /// Default behavior is to do nothing.
    virtual void _light_up_surroundings();

    /// Receive light from the specified LightSource.
    /// The default behavior is to do nothing.
    virtual void _be_lit_by(LightSource& light);

    /// Ratio of desired height to font size.
    static float font_line_to_point_ratio_;

    /// Outline color for walls when drawing on-screen.
    static sf::Color const wall_outline_color_;

  private:
    /// Returns whether the Thing can hold a certain thing.
    virtual ActionResult _can_contain(Thing& thing) const;

    /// Gets this location's maptile.
    virtual MapTile* _get_maptile() const;

    /// Return this thing's description.
    virtual std::string _get_description() const = 0;

    virtual bool _perform_action_activated_by(Entity& entity);
    virtual void _perform_action_collided_with(Thing& thing);
    virtual bool _perform_action_drank_by(Entity& entity);
    virtual bool _perform_action_dropped_by(Entity& entity);
    virtual bool _perform_action_eaten_by(Entity& entity);
    virtual bool _perform_action_picked_up_by(Entity& entity);
    virtual bool _perform_action_put_into(Thing& container);
    virtual bool _perform_action_take_out();
    virtual ActionResult _perform_action_read_by(Entity& entity);
    virtual void _perform_action_attack_hits(Entity& entity);
    virtual bool _perform_action_thrown_by(Entity& entity, Direction direction);
    virtual bool _perform_action_deequipped_by(Entity& entity,
                                               WearLocation& location);
    virtual bool _perform_action_equipped_by(Entity& entity,
                                             WearLocation& location);
    virtual bool _perform_action_unwielded_by(Entity& entity);
    virtual bool _perform_action_wielded_by(Entity& entity);
    virtual bool _perform_action_fired_by(Entity& entity, Direction direction);

    /// Process this Thing for a single tick.
    /// By default, does nothing and returns true.
    /// The function returns false to indicate to its parent that it no longer
    /// exists and should be deleted.
    /// @return true if the Thing continues to exist after the tick;
    ///         false if the Thing ceases to exist.
    virtual bool _do_process();
};

#endif // THING_H
