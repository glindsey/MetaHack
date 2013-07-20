#ifndef SACK_LARGE_H
#define SACK_LARGE_H

#include "Container.h"
#include "CreatableThing.h"

/// Forward declarations
class ThingFactory;

class SackLarge :
  public Container,
  public CreatableThing<SackLarge>
{
  friend class CreatableThing;

  public:
    virtual ~SackLarge();

    // Thing overrides
    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    SackLarge();

  private:
    static SackLarge prototype;
};

#endif // ROCK_H
