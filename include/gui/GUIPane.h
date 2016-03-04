#ifndef GUIPANE_H
#define GUIPANE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

class GUIPane :
  public GUIObject
{
public:
  GUIPane(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
  GUIPane(std::string name, sf::IntRect dimensions);
  virtual ~GUIPane();

protected:

private:
};

#endif // GUIPANE_H
