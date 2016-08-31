#ifndef GUIWINDOWPANE_H
#define GUIWINDOWPANE_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class Window :
    public Object, public ObjectVisitable<Window>
  {
  public:
    Window(StringKey name, Vec2i location, Vec2u size = Vec2u(0, 0));
    Window(StringKey name, sf::IntRect dimensions);
    virtual ~Window();

    /// Get the upper-left corner of this object's child area, relative to
    /// its own upper-left corner, taking into account any decorator
    /// elements added to this window.
    virtual Vec2i get_child_area_location() override final;

    /// Get the size of this window's child area, taking into account any
    /// decorator elements added to this window.
    virtual Vec2u get_child_area_size() override final;

  protected:
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

    virtual void handle_set_flag_(StringKey name, bool enabled) override;

    /// Temporary function used to render the contents of classes derived from
    /// GUIWindow until they can be properly replaced with child controls.
    virtual void render_contents_(sf::RenderTexture& texture, int frame);

    virtual Event::Result handle_event_after_children_(EventDragStarted& event) override;

  private:
    /// Border shape.
    sf::RectangleShape m_border_shape;

    /// Cached state of the "titlebar" flag.
    bool m_titlebar_cached = false;
  };
}; // end namespace metagui

#endif // GUIWINDOWPANE_H
