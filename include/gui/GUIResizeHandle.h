#ifndef GUIRESIZEHANDLE_H
#define GUIRESIZEHANDLE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class ResizeHandle :
    public Object, public ObjectVisitable<ResizeHandle>
  {
    friend class Window;

  public:
    virtual ~ResizeHandle();

  protected:
    explicit ResizeHandle(StringKey name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;
    virtual void handle_parent_size_changed_(sf::Vector2u parent_size) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
