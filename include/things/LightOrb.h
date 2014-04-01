#ifndef LIGHTORB_H
#define LIGHTORB_H

#include "LightSource.h"
#include "Registered.h"

/// A test object, used for testing LightSources.  The final class will just be
/// an "orb".
class LightOrb :
  public LightSource,
  public Registered<LightOrb>
{
  public:
    LightOrb();
    virtual ~LightOrb();
    LightOrb(LightOrb const&) = default;
    LightOrb const& operator=(LightOrb const&) = delete;

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:

  private:
    virtual std::string _get_description() const override;
    static LightOrb prototype;
};

#endif // LIGHTORB_H
