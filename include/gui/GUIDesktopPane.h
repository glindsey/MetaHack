#ifndef GUIDESKTOPPANE_H
#define GUIDESKTOPPANE_H

#include "stdafx.h"

#include "gui/GUIPane.h"

class GUIDesktopPane :
  public GUIPane
{
public:
  GUIDesktopPane(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
  GUIDesktopPane(std::string name, sf::IntRect dimensions);
  virtual ~GUIDesktopPane();

protected:
  virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

private:
};

#endif // GUIDESKTOPPANE_H
