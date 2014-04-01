#ifndef ROCK_H
#define ROCK_H

#include "Thing.h"
#include "Registered.h"

/// Forward declarations
class ThingFactory;

class Rock :
  public Thing,
  public Registered<Rock>
{
  public:
    Rock();
    virtual ~Rock();
    Rock(Rock const&) = default;
    Rock const& operator=(Rock const&) = delete;

    // Thing overrides
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:

  private:
    virtual std::string _get_description() const override;
    static Rock prototype;
};

#endif // ROCK_H
