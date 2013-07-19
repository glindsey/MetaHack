#ifndef CONTAINER_H
#define CONTAINER_H

#include "Thing.h"

/// A Container is a subclass of Thing that has an Inventory.
class Container : public Thing
{
  public:
    virtual ~Container();

    /// Returns a reference to the inventory.
    Inventory& get_inventory();

    /// Returns the size of this Container's inventory.
    /// @return The inventory size, or a special value.
    /// @retval 0 This Container cannot hold other things (but still remains
    ///           a Container).
    /// @retval -1 This Container has an infinite inventory size.
    int const get_inventory_size() const;

    /// Sets the size of this Container's inventory.
    /// @param number Size of the inventory, or a special value.
    /// @see get_inventory_size
    void set_inventory_size(int number);

    /// Returns whether the inventory can hold a certain thing.
    /// Overridden by subclasses; defaults to returning true.
    /// @param thing Thing to check.
    /// @return true if the Container can hold the Thing, false otherwise.
    virtual bool can_contain(Thing& thing) const { return true; }

    /// Gather the ThingIds of this Thing and those underneath it.
    /// This function is used to enumerate all Things on a map in order to
    /// process them for a single game tick.
    virtual void gather_thing_ids(std::vector<ThingId>& ids) override final;

  protected:
    Container();
    Container(const Container& original);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // CONTAINER_H
