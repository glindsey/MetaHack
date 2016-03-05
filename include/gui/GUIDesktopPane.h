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
    explicit DesktopPane(std::string name, sf::Vector2u size);
    virtual ~DesktopPane();

  protected:
    virtual EventResult handle_event_before_children_(sf::Event& event) override final;
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUIDESKTOPPANE_H
