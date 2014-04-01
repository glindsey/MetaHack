#ifndef CHESTWOODEN_H
#define CHESTWOODEN_H

#include "Thing.h"
#include "Registered.h"

/// Forward declarations
class ThingFactory;

class ChestWooden :
  public Thing,
  public Registered<ChestWooden>
{
  public:
    ChestWooden();
    virtual ~ChestWooden();
    ChestWooden(ChestWooden const&) = default;
    ChestWooden const& operator=(ChestWooden const&) = delete;

    // Thing overrides
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

    virtual bool is_openable() const override;
    virtual bool is_lockable() const override;
    virtual bool is_flammable() const override;

  protected:

  private:
    virtual std::string _get_description() const override;
    static ChestWooden prototype;
};

#endif // ROCK_H
