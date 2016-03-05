#ifndef GUIRESIZEHANDLE_H
#define GUIRESIZEHANDLE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class ResizeHandle :
    public Object
  {
    friend class WindowPane;

  public:
    virtual ~ResizeHandle();

  protected:
    explicit ResizeHandle(std::string name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
