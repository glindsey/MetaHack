#ifndef GUISHRINKHANDLE_H
#define GUISHRINKHANDLE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class ShrinkHandle :
    public Object
  {
    friend class WindowPane;

  public:
    virtual ~ShrinkHandle();

  protected:
    explicit ShrinkHandle(std::string name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H