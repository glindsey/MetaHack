#ifndef CONTAINER_H
#define CONTAINER_H

#include "Thing.h"

/// A Container is a subclass of Thing that has an Inventory.
class Container : public Thing
{
  public:
    virtual ~Container();

    /// Get the thing's proper name (if any).
    std::string get_proper_name() const;

    /// Set this thing's proper name.
    void set_proper_name(std::string name);

    /// Return a string that identifies this thing.
    virtual std::string get_name() const;

    /// Return a string that identifies this thing, using indefinite articles.
    virtual std::string get_indef_name() const;

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

    /// Provide light to this Container's surroundings.
    /// The default behavior is to do nothing.
    virtual void light_up_surroundings() override;

    /// Receive light from the specified LightSource.
    /// The default behavior is to do nothing.
    virtual void be_lit_by(LightSource& light) override;

    /// Returns whether the inventory can hold a certain thing.
    /// Overridden by subclasses; defaults to returning true.
    /// @param thing Thing to check.
    /// @return true if the Container can hold the Thing, false otherwise.
    virtual bool can_contain(Thing& thing) const { return true; }

    /// Gather the ThingIds of this Thing and those underneath it.
    /// This function is used to enumerate all Things on a map in order to
    /// process them for a single game tick.
    virtual void gather_thing_ids(std::vector<ThingId>& ids) override final;

    /// Returns true if this container can be opened/closed.
    /// By default, returns false. Derived classes can override this.
    virtual bool is_openable() const;

    /// Returns true if this container can be locked/unlocked.
    virtual bool is_lockable() const;

    /// Get whether this container is opened.
    /// If is_openable() is false, this function always returns true.
    /// @return True if the container is open, false otherwise.
    bool is_open() const;

    /// Set whether this container is open.
    /// If is_openable() is false, this function does nothing.
    /// @return True if the container is open, false otherwise.
    bool set_open(bool open);

    /// Get whether this container is locked.
    /// If is_openable() or is_lockable() is false, this function always
    /// returns false.
    /// @return True if the container is locked, false otherwise.
    bool is_locked() const;

    /// Set whether this container is locked.
    /// If is_openable() or is_lockable() is false, this function does nothing.
    /// @return True if the container is locked, false otherwise.
    bool set_locked(bool open);

  protected:
    Container();
    Container(const Container& original);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // CONTAINER_H
