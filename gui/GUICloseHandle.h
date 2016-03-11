#ifndef GUICLOSEHANDLE_H
#define GUICLOSEHANDLE_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class CloseHandle :
    public Object, public ObjectVisitable<CloseHandle>
  {
    friend class Window;

  public:
    virtual ~CloseHandle();

  protected:
    explicit CloseHandle(StringKey name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;
    virtual void handle_parent_size_changed_(sf::Vector2u parent_size) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
