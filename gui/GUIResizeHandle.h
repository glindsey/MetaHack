#ifndef GUIRESIZEHANDLE_H
#define GUIRESIZEHANDLE_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class ResizeHandle :
    public GUIObject, public GUIObjectVisitable<ResizeHandle>
  {
    friend class Window;

  public:
    virtual ~ResizeHandle();

  protected:
    explicit ResizeHandle(std::string name);
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual void handleParentSizeChanged_(UintVec2 parent_size) override final;

    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventDragStarted& event) override;
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventDragging& event) override;

  private:
    /// Size of the resize handle.
    /// @todo This should be moved into a central config file.
    static unsigned int const s_handle_size = 10;

    /// The starting parent size before the resize drag began.
    UintVec2 m_parent_size_start;

  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
