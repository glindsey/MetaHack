#ifndef GUITITLEBAR_H
#define GUITITLEBAR_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class TitleBar :
    public Object
  {
    friend class WindowPane;

  public:
    virtual ~TitleBar();

  protected:
    explicit TitleBar(std::string name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUITITLEBAR_H