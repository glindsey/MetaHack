#ifndef SACK_LARGE_H
#define SACK_LARGE_H

#include "Registered.h"

/// Forward declarations
class ThingFactory;

class SackLarge :
  public Thing,
  public Registered<SackLarge>
{
  public:
    SackLarge();
    virtual ~SackLarge();
    SackLarge(SackLarge const&) = default;
    SackLarge const& operator=(SackLarge const&) = delete;

    // Thing overrides
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:

  private:
    virtual std::string _get_description() const override;
    static SackLarge prototype;
};

#endif // ROCK_H
