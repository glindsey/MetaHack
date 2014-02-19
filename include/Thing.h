#ifndef THING_H
#define THING_H

#include "ActionResult.h"
#include "BodyPart.h"
#include "Direction.h"
#include "IsType.h"
#include "MapId.h"
#include "ThingId.h"

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

// Forward declarations
class Container;
class Entity;
class MapTile;
class Inventory;
class LightSource;
class ThingFactory;

// Typedef for the factory method.
class Thing;
typedef Thing* (*ThingCreator)();

// Thing is any object in the game, animate or not.
class Thing
{
  friend class ThingFactory;

  /// Qualities that are duplicated when an item is copied.  Also used to
  /// compare two things when combining Aggregates.
  struct Qualities
  {
    int physical_mass;                      ///< Mass of the object
    Direction direction = Direction::None;  ///< Direction the thing is facing.
    bool magic_autolocking;                 ///< True = magic locks on equip
    bool magic_locked;
    bool invisible;
  };

  public:
    virtual ~Thing();

    // Get this thing's number.
    ThingId get_id() const;

    /// Return the location of this thing.
    ThingId get_location_id() const;

    /// Trace this thing's location back to map coordinates, if possible.
    /// This function calls itself recursively, tracing up the location chain
    /// until it gets to a MapTile or to itself.  (The only Thing that is
    /// allowed to have itself as its location is Limbo.)
    ThingId get_root_id() const;

    /// Trace this thing's location back to either a MapTile, an Entity, or
    /// Limbo.  Used to determine who "owns" the current object, if anybody.
    ThingId get_owner_id() const;

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

    /// Set whether this item is magically locked.
    void set_magically_locked(bool locked);

    /// Set whether this item autolocks.
    void set_magically_autolocks(bool autolocks);

    /// Get whether this item is magically locked.
    bool is_magically_locked() const;

    /// Get whether this item autolocks.
    bool magically_autolocks() const;

    /// Get a const pointer to the Qualities struct.
    /// This is used by Aggregates to compare when combining objects.
    Qualities const* get_qualities_pointer() const;

    /// Compare this Thing's type with another one, and return true if they are
    /// identical.
    bool has_same_type_as(Thing const& other) const;

    /// Compare this Thing's qualities with another one, and return true if
    /// they are identical.
    bool has_same_qualities_as(Thing const& other) const;

    /// Return this thing's description.
    /// Calls the overridden _get_description() and adds adjective qualifiers
    /// (such as "fireproof", "waterproof", etc.)
    std::string get_description() const;

    /// Return a string that identifies this thing.
    /// By default, returns "the" and a description of the thing, such as
    /// "the chair".
    /// If it is carried by the player, it'll return "your (thing)".
    /// If it IS the player, it'll return "you".
    /// Likewise, if it is carried by another Entity it'll return
    /// "(Entity)'s (thing)".
    virtual std::string get_name() const;

    /// Return a string that identifies this thing.
    /// By default, returns "a" or "an" and a description of the thing, such as
    /// "a chair" or "an orange".
    /// If it IS the player, it'll return "you".
    /// Unlike get_name, get_indef_name does NOT check for possession.
    virtual std::string get_indef_name() const;

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

    /// Get the quantity this thing represents.
    /// For a Thing that isn't an Aggregate, this is always 1.
    virtual unsigned int get_quantity() const;

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

    /// Choose the proper possessive form.
    /// This function checks to see if this Thing is currently designated as
    /// the player (in the ThingFactory).  If so, it returns "your".  If not,
    /// it returns get_name() + "'s".
    /// @note If you want a possessive pronoun like his/her/its/etc., use
    /// get_possessive_adjective().
    virtual std::string get_possessive() const;

    /// Return the coordinates of the tile representing the thing.
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const;

    /// Add this Thing to a VertexArray to be drawn.
    /// @param vertices Array to add vertices to.
    /// @param use_lighting If true, calculate lighting when adding.
    ///                     If false, store directly w/white bg color.
    /// @param frame Animation frame number.
    void add_vertices_to(sf::VertexArray& vertices,
                         bool use_lighting = true,
                         int frame = 0);

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

    /// Simple check to see if a Thing is a Container.
    bool is_container() const;

    /// Simple check to see if a Thing is an Entity.
    bool is_entity() const;

    /// Simple check to see if a Thing is a MapTile.
    bool is_maptile() const;

    /// Simple check to see if a Thing is opaque.
    virtual bool is_opaque() const;

    /// Provide light to this Thing's surroundings.
    /// The default behavior is to do nothing.
    virtual void light_up_surroundings();

    /// Receive light from the specified LightSource.
    /// The default behavior is to do nothing.
    virtual void be_lit_by(LightSource& light);

    /// Attempt to move this Thing to a location.
    virtual bool move_into(ThingId location_id);

    /// Attempt to move this Thing into a location, by reference.
    virtual bool move_into(Container& location);

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

    /// Process this Thing for a single tick.
    /// By default, does nothing and returns true.
    /// The function returns false to indicate to its caller that it no longer
    /// exists.  This is <i>a courtesy only</i> -- it is up to the Thing to
    /// move itself into Limbo ("this->move_into(TF.limbo_id)") if it
    /// self-destructs!
    /// @return true if the Thing continues to exist after the tick;
    ///         false if the Thing ceases to exist.
    virtual bool do_process();

    /// Gather the ThingIds of this Thing and those underneath it.
    /// This function is used to enumerate all Things on a map in order to
    /// process them for a single game tick.
    virtual void gather_thing_ids(std::vector<ThingId>& ids);

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

    /// Return the type of this thing.
    virtual char const* get_thing_type() const final;

    /// Returns whether this Thing can hold a liquid.
    virtual bool is_liquid_carrier() const;

    /// Returns whether this Thing is flammable.
    virtual bool is_flammable() const;

    /// Returns whether this Thing is corrodible.
    virtual bool is_corrodible() const;

    /// Returns whether this Thing is shatterable.
    virtual bool is_shatterable() const;

  protected:
    /// Constructor, callable only by ThingFactory.
    Thing();

    /// Copy Constructor
    Thing(const Thing& original);

    struct Impl;
    std::unique_ptr<Impl> impl;

    // Set this thing's number.
    void set_id(ThingId id);

    /// Set the location of this thing.
    /// Does no checks, nor does it update the source/target inventories.
    /// Those are the responsibility of the caller.
    void set_location_id(ThingId target);

    /// Static method initializing font sizes.
    static void initialize_font_sizes();

    /// Ratio of desired height to font size.
    static float font_line_to_point_ratio_;

    /// Outline color for walls when drawing on-screen.
    static sf::Color const wall_outline_color_;

  private:
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
};

#endif // THING_H
