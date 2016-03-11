#ifndef GUISHRINKHANDLE_H
#define GUISHRINKHANDLE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class ShrinkHandle :
    public Object, public ObjectVisitable<ShrinkHandle>
  {
    friend class Window;

  public:
    virtual ~ShrinkHandle();

  protected:
    explicit ShrinkHandle(StringKey name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;
    virtual void handle_parent_size_changed_(sf::Vector2u parent_size) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
