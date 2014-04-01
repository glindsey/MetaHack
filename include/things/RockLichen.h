#ifndef ROCKLICHEN_H
#define ROCKLICHEN_H

#include "Blob.h"
#include "Registered.h"

class RockLichen :
  public Blob,
  public Registered<RockLichen>
{
  public:
    RockLichen();
    virtual ~RockLichen();
    RockLichen(RockLichen const&) = default;
    RockLichen const& operator=(RockLichen const&) = delete;

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:


  private:
    virtual std::string _get_description() const override;
    static RockLichen prototype;
};

#endif // ROCKLICHEN_H
