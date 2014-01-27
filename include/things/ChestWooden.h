#ifndef CHESTWOODEN_H
#define CHESTWOODEN_H

#include "Container.h"
#include "CreatableThing.h"

/// Forward declarations
class ThingFactory;

class ChestWooden :
  public Container,
  public CreatableThing<ChestWooden>
{
  friend class CreatableThing;

  public:
    virtual ~ChestWooden();

    // Thing overrides
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

    virtual bool is_openable() const override;
    virtual bool is_lockable() const override;
    virtual bool is_flammable() const override;

  protected:
    ChestWooden();

  private:
    virtual std::string _get_description() const override;
    static ChestWooden prototype;
};

#endif // ROCK_H
