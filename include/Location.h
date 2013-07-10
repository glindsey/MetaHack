#ifndef LOCATION_H
#define LOCATION_H

#include <memory>

// Forward declarations
class Thing;
class Inventory;

// Class indicating something has an inventory it can store stuff in.
class Location
{
  public:
    Location();

    virtual ~Location();

    /// Returns a reference to the inventory.
    Inventory& inventory();

    /// Returns whether the inventory can hold a certain thing.
    virtual bool canContain(Thing* thing) const;

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


#endif // LOCATION_H
