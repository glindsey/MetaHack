#ifndef HUMAN_H
#define HUMAN_H

#include "Biped.h"
#include "Registered.h"

class Human :
  public Biped,
  public Registered<Human>
{
  public:
    Human();
    virtual ~Human();
    Human(Human const&) = default;
    Human const& operator=(Human const&) = delete;

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:

  private:
    virtual std::string _get_description() const override;
    static Human prototype;
};

#endif // HUMAN_H
