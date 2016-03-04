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
  virtual bool _render_self(sf::RenderTexture& texture, int frame) override final;
  virtual std::string _render_contents(sf::RenderTexture& texture, int frame);

private:
  /// Border shape.
  sf::RectangleShape m_border_shape;
};

#endif // GUIPANE_H
