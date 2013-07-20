#ifndef LIGHTORB_H
#define LIGHTORB_H

#include "LightSource.h"
#include "CreatableThing.h"

/// A test object, used for testing LightSources.  The final class will just be
/// an "orb".
class LightOrb :
  public LightSource,
  public CreatableThing<LightOrb>
{
  friend class CreatableThing;

  public:
    virtual ~LightOrb();

    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    LightOrb();

  private:
    static LightOrb prototype;
};

#endif // LIGHTORB_H
