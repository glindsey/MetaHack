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
/// In practice I can't think of a reason for a Thing to be neither an
/// Aggregate nor a Container, unless you wanted to absolutely preclude the
/// possibility of a certain item being consolidated into aggregate groups.
class Aggregate : public Thing
{
  public:
    virtual ~Aggregate();

  protected:
    Aggregate();
    Aggregate(Aggregate const& original);

    /// Split constructor (used by the Splittable template class)
    Aggregate(Aggregate& source, unsigned int number);

    /// Get the quantity this thing represents.
    unsigned int get_quantity() const;

    /// Set the quantity this thing represents.
    void set_quantity(unsigned int quantity);

    /// Return the aggregate's volume.
    virtual int get_size() const override final;

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

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // AGGREGATE_H
