#ifndef GUIWINDOWPANE_H
#define GUIWINDOWPANE_H

#include "stdafx.h"

#include "gui/GUIPane.h"

namespace metagui
{
  class WindowPane :
    public Pane
  {
  public:
    WindowPane(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
    WindowPane(std::string name, sf::IntRect dimensions);
    virtual ~WindowPane();

  protected:
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

    /// Temporary function used to render the contents of classes derived from
    /// GUIWindowPane until they can be properly replaced with child controls.
    virtual void render_contents_(sf::RenderTexture& texture, int frame);

  private:
    /// Border shape.
    sf::RectangleShape m_border_shape;
  };
}; // end namespace metagui

#endif // GUIWINDOWPANE_H
