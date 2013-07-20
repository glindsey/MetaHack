#ifndef ROCK_H
#define ROCK_H

#include "Aggregate.h"
#include "CreatableThing.h"

/// Forward declarations
class ThingFactory;

class Rock :
  public Aggregate,
  public CreatableThing<Rock>
{
  friend class CreatableThing;

  public:
    virtual ~Rock();

    // Thing overrides
    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    Rock();
    Rock(Rock const& original);

  private:
    static Rock prototype;
};

#endif // ROCK_H
