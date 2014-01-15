#ifndef AGGREGATE_H
#define AGGREGATE_H

#include "Thing.h"

#include <memory>

/// An Aggregate is a subclass of Thing that means a single Thing instance can
/// represent one or more of a particular object.  For logistical reasons,
/// a Thing can be an Aggregate, a Container, or neither, but it can NOT be
/// both (because the members of the Aggregate must be identical, and having
/// individual inventories precludes this).
///
/// The Aggregate class is designed for small, numerous things: coins, pebbles,
/// candles, darts, arrows, et cetera, which would otherwise be cumbersome to
/// list in an inventory.
///
/// In practice I can't think of a reason for a Thing to be neither an
/// Aggregate nor a Container, unless you wanted to absolutely preclude the
/// possibility of a certain item being consolidated into aggregate groups.
///
/// For the ThingFactory to "split" an aggregate, it does the following:
/// 1. Calls original_thing.clone() to get a copy of the original Aggregate.
/// 2. Calls original_thing.get_quantity() to get the thing's quantity.
/// 3. Splits the quantity into two variables, old_quantity and new_quantity.
/// 4. Calls new_thing.set_quantity(new_quantity).
/// 5. Calls old_thing.set_quantity(old_quantity).
class Aggregate : public Thing
{
  public:
    virtual ~Aggregate();

    /// Return this aggregate's mass.
    virtual int get_mass() const override final;

    /// Return a string that identifies this thing.
    /// This is similar to Thing::get_name except it also takes the
    /// quantity into account.
    /// @see Thing::get_name
    virtual std::string get_name() const override final;

    /// Return a string that identifies this thing.
    /// This is similar to Thing::get_indef_name except it also takes the
    /// quantity into account.
    /// @see Thing::get_indef_name
    virtual std::string get_indef_name() const override final;

    /// Get the quantity this thing represents.
    virtual unsigned int get_quantity() const;

    /// Set the quantity this thing represents.
    void set_quantity(unsigned int quantity);

    /// Attempt to move this Aggregate into a location, by reference.
    /// @todo If an equivalent Aggregate exists in the new location, combine the
    ///       two into a single object.
    virtual bool move_into(Container& location) override final;


  protected:
    Aggregate();
    Aggregate(Aggregate const& original);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // AGGREGATE_H
