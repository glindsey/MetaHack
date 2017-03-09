#ifndef GUISHRINKHANDLE_H
#define GUISHRINKHANDLE_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class ShrinkHandle :
    public Object, public ObjectVisitable<ShrinkHandle>
  {
    friend class Window;

  public:
    virtual ~ShrinkHandle();

  protected:
    explicit ShrinkHandle(std::string name);
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual void handleParentSizeChanged_(Vec2u parent_size) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
