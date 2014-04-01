#ifndef BIPED_H
#define BIPED_H

#include "Entity.h"

class Biped :
  public Entity
{
  public:
    virtual ~Biped();

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;
    virtual unsigned int get_bodypart_number(BodyPart part) const override;
    virtual std::string get_bodypart_name(BodyPart part) const override;
    virtual std::string get_bodypart_plural(BodyPart part) const override;

  protected:
    Biped();

  private:
};

#endif // BIPED_H
