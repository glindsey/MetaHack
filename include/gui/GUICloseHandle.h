#ifndef GUICLOSEHANDLE_H
#define GUICLOSEHANDLE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class CloseHandle :
    public Object
  {
    friend class WindowPane;

  public:
    virtual ~CloseHandle();

  protected:
    explicit CloseHandle(std::string name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
