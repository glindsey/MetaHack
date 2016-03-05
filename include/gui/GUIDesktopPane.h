#ifndef GUIDESKTOPPANE_H
#define GUIDESKTOPPANE_H

#include "stdafx.h"

#include "gui/GUIPane.h"

namespace metagui
{
  class DesktopPane :
    public Pane
  {
  public:
    DesktopPane(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
    DesktopPane(std::string name, sf::IntRect dimensions);
    virtual ~DesktopPane();

  protected:
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUIDESKTOPPANE_H
