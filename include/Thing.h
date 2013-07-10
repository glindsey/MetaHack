#ifndef THING_H
#define THING_H

#include "ActionResult.h"
#include "Direction.h"
#include "MapId.h"
#include "ThingId.h"

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

// Forward declarations
class Entity;
class MapTile;
class Inventory;
class ThingFactory;

// Typedef for the factory method.
class Thing;
typedef Thing* (*ThingCreator)();

// Thing is any object in the game, animate or not.
class Thing
{
  friend class ThingFactory;

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

    /// Get the thing's proper name (if any).
    std::string get_proper_name() const;

    /// Set this thing's proper name.
    void set_proper_name(std::string name);

    /// Return this thing's volume.
    /// Volumes are represented as the diameter of an equivalent sphere, where
    /// a diameter of 1 would be a single tile, 2 would be two tiles, etc.
    int get_size() const;

    /// Return this thing's mass.
    int get_mass() const;

    /// Get the quantity this thing represents.
    unsigned int get_quantity() const;

    /// Set this thing's volume.
    void set_size(int volume);

    /// Set this thing's mass.
    void set_mass(int mass);

    /// Set the quantity this thing represents.
    void set_quantity(unsigned int quantity);

    /// Return a string that identifies this thing.
    /// By default, this returns the thing's proper name.  If the thing
    /// does not have a proper name, it returns "the" and a description
    /// of the thing, such as "the chair".
    /// If it is carried by the player, it'll return "your (thing)".
    /// If it IS the player, it'll return "you".
    /// Likewise, if it is carried by another Entity it'll return
    /// "(Entity)'s (thing)".
    std::string get_name() const;

    /// Return a string that identifies this thing.
    /// By default, this returns the thing's proper name.  If the thing
    /// does not have a proper name, it returns "a" or "an" and a description
    /// of the thing, such as "a chair" or "an orange".
    /// If it IS the player, it'll return "you".
    /// Unlike get_name, get_indef_name does NOT check for possession.
    std::string get_indef_name() const;

    /// Choose which verb form to use based on first/second/third person.
    /// This function checks to see if this Thing is currently designated as
    /// the player (in the ThingFactory).  If so, it returns the string passed
    /// as verb2; otherwise, it returns the string passed as verb3.
    /// @param verb2 The second person verb form, such as "shake"
    /// @param verb3 The third person verb form, such as "shakes"
    std::string const& choose_verb(std::string const& verb2,
                                   std::string const& verb3) const;

    /// Return this object's plural.
    /// By default, returns get_description() plus "s", but this can be
    /// overridden by child classes (for example "djinn" -> "djinni").
    virtual std::string get_plural() const;

    /// Return this thing's description.
    virtual std::string get_description() const = 0;

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


    /// Attempt to move this Thing to a location.
    virtual bool move_into(ThingId location_id);

    /// Attempt to move this Thing into a location, by reference.
    virtual bool move_into(Thing& location);

    /// Return whether or not this thing can move from its current location.
    /// The default behavior for this is to return true.
    virtual bool can_be_moved() const;

    /// Return whether or not this thing can be activated by this Entity.
    /// The default behavior for this is to return false.
    virtual bool can_be_activated_by(Entity const& entity) const;

    /// Return whether or not this thing can be drank by this Entity.
    /// The default behavior for this is to return false.
    virtual bool can_be_drank_by(Entity const& entity) const;

    /// Return whether or not this thing can be eaten by this Entity.
    /// The default behavior for this is to return false.
    virtual bool can_be_eaten_by(Entity const& entity) const;

    /// Return whether or not this thing can be put into this container.
    /// The default behavior for this is to return true.
    virtual bool can_be_put_into(Thing const& container) const;

    /// Return whether or not this thing can be taken out of its container.
    /// The default behavior for this is to return true.
    virtual bool can_be_taken_out() const;

    /// Return whether or not this thing can be read by this Entity.
    /// The default behavior for this is to return false.
    virtual bool can_be_read_by(Entity const& entity) const;

    /// Return whether or not this thing can be deequipped from this Thing.
    /// The default behavior for this is to return false.
    virtual bool can_be_deequipped_from(Thing const& thing) const;

    /// Return whether or not this thing can be thrown.
    /// The default behavior for this is to return true.
    virtual bool can_be_thrown_by(Entity const& entity) const;

    /// Return whether or not this thing can be wielded by this Entity.
    /// The default behavior for this is to return true.
    virtual bool can_be_wielded_by(Entity const& entity) const;

    /// Return whether or not this thing can be equipped on this Thing.
    /// The default behavior for this is to return false.
    virtual bool can_be_equipped_on(Thing const& thing) const;

    /// Return whether or not this thing can be fired by this Entity.
    /// The default behavior for this is to return false.
    virtual bool can_be_fired_by(Entity const& entity) const;

    /// Return whether or not this thing can be mixed with another Thing.
    /// The default behavior for this is to return false.
    virtual bool can_be_mixed_with(Thing const& thing) const;

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
    void gather_thing_ids(std::vector<ThingId>& ids);

    /// Perform an action when this thing is activated.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    virtual bool do_action_activated_by(Entity& entity);

    /// Perform an action when this thing collides with another thing.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    /// @todo How does one abort a collision?  Does this make sense at all?
    virtual bool do_action_collided_with(Thing& thing);

    /// Perform an action when this thing is eaten.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    virtual bool do_action_drank_by(Entity& entity);

    /// Perform an action when this thing is dropped.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_dropped_by(Entity& entity);

    /// Perform an action when this thing is eaten.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    virtual bool do_action_eaten_by(Entity& entity);

    /// Perform an action when this thing is picked up.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_picked_up_by(Entity& entity);

    /// Perform an action when this thing is put into another thing.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_put_into(Thing& container);

    /// Perform an action when this thing is taken out its container.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_take_out();

    /// Perform an action when this thing is read.
    /// If this function returns Failure, the action is aborted.
    /// The default behavior is to do nothing and return Failure.
    virtual ActionResult do_action_read_by(Entity& entity);

    /// Perform an action when this thing is de-equipped (taken off).
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_deequipped_from(Thing& thing);

    /// Perform an action when this thing is thrown.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_thrown_by(Entity& entity, Direction direction);

    /// Perform an action when this thing is equipped.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    virtual bool do_action_equipped_onto(Thing& thing);

    /// Perform an action when this thing is wielded.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return true.
    virtual bool do_action_wielded_by(Entity& entity);

    /// Perform an action when this thing is fired.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    virtual bool do_action_fired_by(Entity& entity, Direction direction);

    /// Return the type of this thing.
    virtual char const* get_thing_type() const final;

    /// Returns a reference to the inventory.
    Inventory& get_inventory();

    /// Returns whether this Thing's inventory can hold a liquid.
    virtual bool is_liquid_carrier() const;

    /// Returns the size of this Thing's inventory.
    /// The size is a total volume count.
    /// If 0, this Thing cannot hold other things.
    /// If -1, this Thing has an infinite inventory size.
    virtual int get_inventory_size() const;

    /// Returns whether the inventory can hold a certain thing.
    virtual bool can_contain(Thing& thing) const;

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
};

#endif // THING_H
