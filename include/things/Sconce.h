#ifndef SCONCE_H
#define SCONCE_H

#include "LightSource.h"
#include "Registered.h"

/// A mounted torch.
class Sconce :
  public LightSource,
  public Registered<Sconce>
{
  public:
    Sconce();
    virtual ~Sconce();
    Sconce(Sconce const&) = default;
    Sconce const& operator=(Sconce const&) = delete;

    // Thing overrides
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:

  private:
    virtual bool _do_process() override;
    virtual std::string _get_description() const override;
    static Sconce prototype;
};

#endif // LIGHTORB_H
