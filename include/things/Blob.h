#ifndef BLOB_H
#define BLOB_H

#include "Entity.h"

#include "ThingId.h"

/// A Blob is an Entity type with no discernible features whatsoever.
class Blob :
  public Entity
{
  friend class ThingFactory;

  public:
    virtual ~Blob();

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override = 0;
    virtual unsigned int get_bodypart_number(BodyPart part) const override;
    virtual std::string get_bodypart_name(BodyPart part) const override;
    virtual std::string get_bodypart_plural(BodyPart part) const override;
    virtual bool can_currently_see() const override;
    virtual bool can_currently_move() const override;

  protected:
    Blob();

  private:
};

#endif // BLOB_H
