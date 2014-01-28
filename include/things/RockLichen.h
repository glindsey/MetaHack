#ifndef ROCKLICHEN_H
#define ROCKLICHEN_H

#include "Blob.h"
#include "CreatableThing.h"

class RockLichen :
  public Blob,
  public CreatableThing<RockLichen>
{
  friend class CreatableThing;

  public:
    virtual ~RockLichen();

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    RockLichen();

  private:
    virtual std::string _get_description() const override;
    static RockLichen prototype;
};

#endif // ROCKLICHEN_H
