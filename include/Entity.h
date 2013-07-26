#ifndef ENTITY_H
#define ENTITY_H

#include "Action.h"
#include "AttributeSet.h"
#include "BodyPart.h"
#include "Container.h"
#include "Direction.h"
#include "Gender.h"
#include "MapTileType.h"
#include "Thing.h"
#include "ThingId.h"

#include <boost/dynamic_bitset.hpp>
#include <set>

/// Interface class representing something that can perform actions.
class Entity :
  public Container
{
  friend class ThingFactory;

  public:
    virtual ~Entity();

    /// Return the coordinates of the tile representing the entity.
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

    /// Get the number of game cycles until this Entity can process a new
    /// command.
    int get_busy_counter() const;

    /// Set the entity's gender.
    /// Simply changes the object's member (so to speak); no special actions
    /// are performed.
    void set_gender(Gender g);

    /// Get the entity's true gender, without checking for player.
    Gender get_true_gender() const;

    /// Get the entity's gender, checking if the entity is the player.
    /// If the entity is the active player, it will return Gender::SecondPerson
    /// instead of whatever gender is set.
    Gender get_gender() const;

    /// Set the direction the entity is facing.
    void set_facing_direction(Direction d);

    /// Get the direction the entity is facing.
    Direction get_facing_direction() const;

    /// Traverse the line of sight to a map tile, setting visibility
    /// for the tiles between.
    /// @warning Assumes entity is on a map, and that the ending coordinates
    ///          are valid for the map the entity is on!
    void traverseLineOfSightTo(int xEnd, int yEnd);

    /// Return whether the Entity can see the requested Thing.
    bool can_see(Thing& thing);

    /// Return whether the Entity can see the requested tile.
    bool can_see(int x, int y);

    /// Return whether the Entity can see the requested tile.
    bool can_see(sf::Vector2i coords);

    /// Find out which tiles on the map can be seen by this Entity.
    void find_seen_tiles();

    /// Get the remembered tile type at the specified coordinates.
    MapTileType get_memory_at(int x, int y) const;

    /// Get the remembered tile type at the specified coordinates.
    MapTileType get_memory_at(sf::Vector2i coords) const;

    /// Add the memory of a particular tile to a VertexArray.
    void add_memory_vertices_to(sf::VertexArray& vertices,
                                int x, int y);

    /// Check if the Entity can move in the specified direction.
    virtual bool can_move(Direction direction);

    /// Attempt to move this Entity to a new location.
    virtual bool move_into(ThingId location_id) override final;

    /// Get the number of a particular body part the Entity has.
    virtual unsigned int get_bodypart_number(BodyPart part) const = 0;

    /// Get the appropriate body part name for the Entity.
    virtual std::string get_bodypart_name(BodyPart part) const = 0;

    /// Get the appropriate body part plural for the Entity.
    virtual std::string get_bodypart_plural(BodyPart part) const = 0;

    /// Get the appropriate description for a body part.
    /// This takes the body part name and the number referencing the particular
    /// part and comes up with a description.
    /// For example, for most creatures with two hands, hand #0 will be the
    /// "right hand" and hand #1 will be the "left hand".
    /// In most cases the default implementation here will work, but if a
    /// creature has (for example) a strange configuration of limbs this can be
    /// overridden.
    virtual std::string get_bodypart_description(BodyPart part,
                                                 unsigned int number);

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

    /// Return the attribute set for this Entity.
    virtual AttributeSet& get_attributes();

    /// Return the attribute set for this Entity.
    virtual AttributeSet const& get_attributes() const;

    /// Queue an action for this Entity to perform.
    void queue_action(Action action);

    /// Return whether this is an action pending for this Entity.
    bool pending_action();

    /// Process this Entity for one tick.
    virtual bool do_process() override final;

    /// Return whether a Thing is wielded by this Entity.
    /// This is used by InventoryArea to show wielded status.
    /// @param[in] thing_id Thing to check
    /// @return true if the Thing is wielded by the Entity.
    bool is_wielded(ThingId thing_id);

    /// Return whether a Thing is wielded by this Entity.
    /// This is used by InventoryArea to show wielded status.
    /// @param[in] thing_id Thing to check
    /// @param[out] number Hand number it is wielded in.
    /// @return true if the Thing is wielded by the Entity.
    bool is_wielded(ThingId thing_id, unsigned int* number_ptr);

    /// Return whether a Thing is within reach of the Entity.
    /// @param[in] thing_id Thing to check
    /// @return true if the Thing is in the Entity's inventory or is at the
    ///         same location as the Entity, false otherwise.
    bool can_reach(ThingId thing_id);

    /// Return whether the Entity can drink the requested Thing.
    /// The base method checks to make sure the Thing is a liquid, but beyond
    /// that, assumes it can drink anything.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing_id Thing to try to drink
    /// @param[out] action_time The time it will take to drink it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_drink(ThingId thing_id, unsigned int& action_time);

    /// Attempt to drink a thing.
    /// @param[in] thing_id Thing to try to drink
    /// @param[out] action_time The time it took to drink it.
    /// @return true if object is drank, false if not
    bool drink(ThingId thing_id, unsigned int& action_time);

    /// Return whether the Entity can drop the requested Thing.
    /// The base method checks to make sure the Entity is actually holding the
    /// thing in its Inventory.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing_id Thing to try to drop
    /// @param[out] action_time The time it will take to drop it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_drop(ThingId thing_id,
                                  unsigned int& action_time);

    /// Attampt to drop a thing.
    /// @param[in] thing_id Thing to try to drop
    /// @param[out] action_time The time it took to drop it.
    /// @return true if object is dropped, false if not
    bool drop(ThingId thing_id, unsigned int& action_time);

    /// Return whether the Entity can eat the requested Thing.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing_id Thing to try to eat
    /// @param[out] action_time The time it will take to eat it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_eat(ThingId thing_id, unsigned int& action_time);

    /// Attempt to eat a thing.
    bool eat(ThingId thing_id, unsigned int& action_time);

    /// Return whether the Entity can mix these two Things.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing1_id First thing to mix.
    /// @param[in] thing2_id Second thing to mix.
    /// @param[out] action_time The time it will take to mix.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_mix(ThingId thing1_id, ThingId thing2_id,
                                 unsigned int& action_time);

    /// Return whether the Entity can put thing into container.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing_id Thing to put in.
    /// @param[in] container_id Thing to put it into.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_put_into(ThingId thing_id, ThingId container_id,
                                      unsigned int& action_time);

    /// Attempt to put a thing into a container.
    bool put_into(ThingId thing_id, ThingId container_id,
                  unsigned int& action_time);

    /// Return whether the Entity can take a thing out of its container.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing_id Thing to take out.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_take_out(ThingId thing_id,
                                      unsigned int& action_time);

    /// Attempt to take a thing out of its container.
    bool take_out(ThingId thing_id, unsigned int& action_time);

    /// Attempt to mix two things.
    bool mix(ThingId thing1_id, ThingId thing2_id, unsigned int& action_time);

    /// Attempt to move in a particular direction.
    /// @param[in] direction Direction to move in
    /// @param[in] turn If true, turn prior to moving.  If false, continue to
    ///                 face the same direction.
    /// @param[out] action_time The time it took to move
    /// @return true on success; false if the move was prevented.
    virtual bool move(Direction direction, bool turn,
                      unsigned int& action_time);

    /// Return whether the Entity can pick up the requested Thing.
    /// The base method checks to make sure the Thing is at the same location
    /// as the Entity, and that the Entity's inventory can contain it.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing_id Thing to try to pick up
    /// @param[out] action_time The time it will take to pick it up
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_pick_up(ThingId thing_id,
                                     unsigned int& action_time);

    /// Attempt to pick a thing up.
    /// @param[in] thing_id Thing to try to pick up
    /// @param[out] action_time The time it took to pick up
    /// @return true if object is picked up, false if not
    bool pick_up(ThingId thing_id, unsigned int& action_time);

    virtual ActionResult can_read(ThingId thing_id, unsigned int& action_time);

    bool read(ThingId thing_id, unsigned int& action_time);

    virtual ActionResult can_toss(ThingId thing_id, unsigned int& action_time);

    /// Attempt to toss/throw a thing in a particular direction.
    bool toss(ThingId thing_id, Direction& direction,
              unsigned int& action_time);

    virtual ActionResult can_wear(ThingId thing_id, unsigned int& action_time);

    /// Attempt to wear a thing.
    bool wear(ThingId thing_id, unsigned int& action_time);

    virtual ActionResult can_wield(ThingId thing_id,
                                   unsigned int hand,
                                   unsigned int& action_time);

    /// Attempt to wield a thing.
    /// @param[in] thing_id Thing to wield, or 0 if unwielding everything.
    /// @param[in] hand Hand to wield it in.
    /// @param[out] action_time Time it takes to wield.
    bool wield(ThingId thing_id,
               unsigned int hand,
               unsigned int& action_time);

  protected:
    Entity();
    Entity(Entity const& original);

    /// Decrement the busy counter if greater than 0.
    /// Returns true if the counter has reached 0, false otherwise.
    bool dec_busy_counter();

    /// Set the busy counter to a value.
    void set_busy_counter(int value);

    /// Perform the recursive visibility scan for an octant.
    /// Used by find_seen_tiles.
    void do_recursive_visibility(int octant,
                                 int depth,
                                 float slope_A,
                                 float slope_B);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // ENTITY_H
