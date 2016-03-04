#ifndef GUIWINDOWPANE_H
#define GUIWINDOWPANE_H

#include "stdafx.h"

#include "gui/GUIPane.h"

class GUIWindowPane :
  public GUIPane
{
public:
  GUIWindowPane(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
  GUIWindowPane(std::string name, sf::IntRect dimensions);
  virtual ~GUIWindowPane();

protected:
  virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  /// Temporary function used to render the contents of classes derived from
  /// GUIWindowPane until they can be properly replaced with child controls.
  virtual std::string render_contents_(sf::RenderTexture& texture, int frame);

private:
  /// Border shape.
  sf::RectangleShape m_border_shape;
};

#endif // GUIPANE_H
