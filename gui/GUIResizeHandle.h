#ifndef GUIRESIZEHANDLE_H
#define GUIRESIZEHANDLE_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class ResizeHandle :
    public Object, public ObjectVisitable<ResizeHandle>
  {
    friend class Window;

  public:
    virtual ~ResizeHandle();

  protected:
    explicit ResizeHandle(std::string name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;
    virtual void handle_parent_size_changed_(Vec2u parent_size) override final;

    virtual Event::Result handle_event_after_children_(EventDragStarted& event) override;
    virtual Event::Result handle_event_after_children_(EventDragging& event) override;

  private:
    /// Size of the resize handle.
    /// @todo This should be moved into a central config file.
    static unsigned int const s_handle_size = 10;

    /// The starting parent size before the resize drag began.
    Vec2u m_parent_size_start;

  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
