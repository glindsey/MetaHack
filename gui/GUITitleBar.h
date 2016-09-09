#ifndef GUITITLEBAR_H
#define GUITITLEBAR_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class TitleBar :
    public Object, public ObjectVisitable<TitleBar>
  {
    friend class Window;

  public:
    virtual ~TitleBar();

  protected:
    explicit TitleBar(std::string name);
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;
    virtual void handle_parent_size_changed_(Vec2u parent_size) override final;

  private:
  };
}; // end namespace metagui

#endif // GUITITLEBAR_H
