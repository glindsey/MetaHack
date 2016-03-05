#ifndef GUIPANE_H
#define GUIPANE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class Pane :
    public Object
  {
  public:
    explicit Pane(std::string name, sf::Vector2i location = sf::Vector2i(0, 0), sf::Vector2u size = sf::Vector2u(0, 0));
    Pane(std::string name, sf::IntRect dimensions);
    virtual ~Pane();

  protected:

  private:
  };
}; // end namespace metagui

#endif // GUIPANE_H
