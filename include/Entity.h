#ifndef ENTITY_H
#define ENTITY_H

#include "Action.h"
#include "AttributeSet.h"
#include "BodyPart.h"
#include "Direction.h"
#include "Gender.h"
#include "MapTileType.h"
#include "Thing.h"

#include <boost/dynamic_bitset.hpp>
#include <set>

/// Forward declarations
class AIStrategy;

/// Interface class representing something that can perform actions.
class Entity :
  public Thing
{
  friend class AIStrategy;
  friend class ThingFactory;

  public:
    virtual ~Entity();

    /// Return a string that identifies this thing.
    /// Extends Thing::get_name() to handle hit points, so values 0 or below
    /// return "the dead (thing)" or "(thing)'s corpse".
    virtual std::string get_name() const override;

    /// Return a string that identifies this thing.
    /// Extends Thing::get_def_name() to handle hit points, so values 0 or below
    /// return "the dead (thing)" or "(thing)'s corpse".
    virtual std::string get_def_name() const;

    /// Return a string that identifies this thing.
    /// Extends Thing::get_indef_name() to handle hit points, so values 0 or below
    /// return "a dead (thing)" or "(thing)'s corpse".
    virtual std::string get_indef_name() const override;

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

    /// Traverse the line of sight to a map tile, setting visibility
    /// for the tiles between.
    /// @warning Assumes entity is on a map, and that the ending coordinates
    ///          are valid for the map the entity is on!
    void traverseLineOfSightTo(int xEnd, int yEnd);

    /// Return whether the Entity can see the requested Thing.
    bool can_see(Thing& thing);

    /// Return whether the Entity can see the requested tile.
    bool can_see(sf::Vector2i coords);

    /// Return whether the Entity can see the requested tile.
    bool can_see(int x, int y);

    /// Find out which tiles on the map can be seen by this Entity.
    /// This method uses a recursive raycasting algorithm to figure out what
    /// can be seen at a particular position.
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
    virtual bool move_into(std::shared_ptr<Thing> location) override final;

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

    /// Return whether this Entity is the current player.
    virtual bool is_player() const override;

    /// Queue an action for this Entity to perform.
    void queue_action(Action action);

    /// Return whether this is an action pending for this Entity.
    bool pending_action() const;

    /// Set the AI strategy associated with this Entity.
    /// The Entity assumes responsibility for maintenance of the new object.
    /// Any old strategy in use will be discarded.
    /// @note While it is possible to use the same AIStrategy instance with
    ///       more than one Entity, doing this with probably not result in the
    ///       desired behavior, as the AIStrategy may maintain state information
    ///       which would be shared across multiple Entities.  However, this
    ///       MIGHT be desirable if dealing with an Entity type with a hive
    ///       mind (thus sharing all of its knowledge across the species).
    /// @param[in] strategy_ptr Pointer to a newly created AIStrategy object.
    /// @return True if strategy was set; false if not (e.g. if the pointer
    ///         passed in was not valid).
    bool set_ai_strategy(AIStrategy* strategy_ptr);

    /// Return whether a Thing is wielded by this Entity.
    /// This is used by InventoryArea to show wielded status.
    /// @param[in] thing Thing to check
    /// @return true if the Thing is wielded by the Entity.
    bool is_wielding(Thing& thing);

    /// Return whether a Thing is wielded by this Entity.
    /// This is used by InventoryArea to show wielded status.
    /// @param[in] thing Thing to check
    /// @param[out] number Hand number it is wielded in.
    /// @return true if the Thing is wielded by the Entity.
    bool is_wielding(Thing& thing, unsigned int& number_ref);

    /// Return whether a Thing is equipped (worn) by this Entity.
    /// @param[in] thing Thing to check
    /// @return true if the Thing is being worn.
    bool has_equipped(Thing& thing);

    /// Return whether a Thing is being worn by this Entity.
    /// @param[in] thing Thing to check
    /// @param[out] location of the worn Thing, if worn
    /// @return true if the Thing is being worn.
    bool has_equipped(Thing& thing, WearLocation& location);

    /// Return whether a Thing is within reach of the Entity.
    /// @param[in] thing Thing to check
    /// @return true if the Thing is in the Entity's inventory or is at the
    ///         same location as the Entity, false otherwise.
    bool can_reach(Thing& thing);

    /// Attempt to attack a thing.
    /// @param[in] thing Thing to attack.
    /// @param[out] action_time The time it took to attack it.
    /// @return true if attack was performed (whether it succeeded or not),
    ///         false if it wasn't performed
    bool attack(Thing& thing, unsigned int& action_time);

    /// Return whether the Entity can drink the requested Thing.
    /// The base method checks to make sure the Thing is a liquid, but beyond
    /// that, assumes it can drink anything.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing Thing to try to drink
    /// @param[out] action_time The time it will take to drink it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_drink(Thing& thing, unsigned int& action_time);

    /// Attempt to drink a thing.
    /// @param[in] thing Thing to try to drink
    /// @param[out] action_time The time it took to drink it.
    /// @return true if object is drank, false if not
    bool drink(Thing& thing, unsigned int& action_time);

    /// Return whether the Entity can drop the requested Thing.
    /// The base method checks to make sure the Entity is actually holding the
    /// thing in its Inventory.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing Thing to try to drop
    /// @param[out] action_time The time it will take to drop it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_drop(Thing& thing,
                                  unsigned int& action_time);

    /// Attampt to drop a thing.
    /// @param[in] thing Thing to try to drop
    /// @param[out] action_time The time it took to drop it.
    /// @return true if object is dropped, false if not
    bool drop(Thing& thing, unsigned int& action_time);

    /// Return whether the Entity can eat the requested Thing.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing Thing to try to eat
    /// @param[out] action_time The time it will take to eat it
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_eat(Thing& thing, unsigned int& action_time);

    /// Attempt to eat a thing.
    bool eat(Thing& thing, unsigned int& action_time);

    /// Return whether the Entity can mix these two Things.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing1 First thing to mix.
    /// @param[in] thing2 Second thing to mix.
    /// @param[out] action_time The time it will take to mix.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_mix(Thing& thing1,
                                 Thing& thing2,
                                 unsigned int& action_time);

    /// Return whether the Entity can put thing into container.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing Thing to put in.
    /// @param[in] container Thing to put it into.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_put_into(Thing& thing,
                                      Thing& container,
                                      unsigned int& action_time);

    /// Attempt to put a thing into a container.
    bool put_into(Thing& thing,
                  std::shared_ptr<Thing> container_sptr,
                  unsigned int& action_time);

    /// Return whether the Entity can take a thing out of its container.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing Thing to take out.
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_take_out(Thing& thing,
                                      unsigned int& action_time);

    /// Attempt to take a thing out of its container.
    bool take_out(Thing& thing, unsigned int& action_time);

    /// Attempt to mix two things.
    bool mix(Thing& thing1,
             Thing& thing2,
             unsigned int& action_time);

    /// Attempt to move in a particular direction.
    /// @param[in] direction Direction to move in
    /// @param[out] action_time The time it took to move
    /// @return true on success; false if the move was prevented.
    virtual bool move(Direction direction,
                      unsigned int& action_time);

    /// Return whether the Entity can pick up the requested Thing.
    /// The base method checks to make sure the Thing is at the same location
    /// as the Entity, and that the Entity's inventory can contain it.
    /// It is recommended that any derived classes first call the base method
    /// before proceeding with their own checks.
    /// @param[in] thing Thing to try to pick up
    /// @param[out] action_time The time it will take to pick it up
    /// @return ActionResult indicating what happened.
    virtual ActionResult can_pick_up(Thing& thing,
                                     unsigned int& action_time);

    /// Attempt to pick a thing up.
    /// @param[in] thing Thing to try to pick up
    /// @param[out] action_time The time it took to pick up
    /// @return true if object is picked up, false if not
    bool pick_up(Thing& thing, unsigned int& action_time);

    virtual ActionResult can_read(Thing& thing, unsigned int& action_time);

    bool read(Thing& thing, unsigned int& action_time);

    virtual ActionResult can_toss(Thing& thing, unsigned int& action_time);

    /// Attempt to toss/throw a thing in a particular direction.
    bool toss(Thing& thing,
              Direction& direction,
              unsigned int& action_time);

    virtual ActionResult can_deequip(Thing& thing,
                                     unsigned int& action_time);

    /// Attempt to de-equip (remove) a thing.
    bool deequip(Thing& thing, unsigned int& action_time);

    virtual ActionResult can_equip(Thing& thing, unsigned int& action_time);

    /// Attempt to equip (wear) a thing.
    bool equip(std::shared_ptr<Thing> thing, unsigned int& action_time);

    virtual ActionResult can_wield(Thing& thing,
                                   unsigned int hand,
                                   unsigned int& action_time);

    /// Attempt to wield a thing.
    /// @param[in] thing Thing to wield, or empty ptr if unwielding everything.
    /// @param[in] hand Hand to wield it in.
    /// @param[out] action_time Time it takes to wield.
    bool wield(std::shared_ptr<Thing> thing,
               unsigned int hand,
               unsigned int& action_time);

    /// Return whether this Entity can currently see.
    /// @todo Implement blindness counter, blindness due to wearing blindfold,
    ///       et cetera.
    virtual bool can_currently_see() const;

    /// Return whether this Entity can currently move.
    /// @todo Implement paralysis counter, and/or other reasons to be immobile.
    virtual bool can_currently_move() const;

    /// Provide light to this Entity's surroundings.
    /// Calls light_up_surroundings() for each Thing in its inventory,
    /// regardless of whether the Entity is opaque.
    /// @todo Change this so only wielded or worn items can contribute to
    ///       lighting the surroundings, unless the Entity is
    ///       semi-transparent. Might be a pain in the ass to players,
    ///       though, even if it is a bit more realistic.
    virtual void light_up_surroundings();

    /// Receive light from the specified LightSource.
    /// The default behavior is to pass the light source to the location.
    /// @todo Change this so only wielded or worn items can contribute to
    ///       lighting the surroundings, unless the Entity is
    ///       semi-transparent. Might be a pain in the ass to players,
    ///       though, even if it is a bit more realistic.
    virtual void be_lit_by(LightSource& light);

  protected:
    Entity();
    Entity(Entity const& original);

    /// Process this Entity for one tick.
    virtual bool _do_process() override final;

    /// Do any subclass-specific processing; called by _do_process().
    /// This function is particularly useful if the subclass is able to do
    /// specialized actions such as rise from the dead after a time (as in
    /// NetHack trolls).
    /// @warning In order to support the aforementioned rising from the dead,
    ///          this function is called <i>regardless of the Entity's HP</>!
    ///          Keep this in mind when implementing specialized behavior.
    virtual void _do_process_specific();

    /// Decrement the busy counter if greater than 0.
    /// Returns true if the counter has reached 0, false otherwise.
    bool dec_busy_counter();

    /// Set the busy counter to a value.
    void set_busy_counter(int value);

    /// Get a reference to this Entity's map memory.
    std::vector<MapTileType>& get_map_memory();

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
