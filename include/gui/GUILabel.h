#ifndef GUILABEL_H
#define GUILABEL_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class Label :
    public Object
  {
  public:
    Label(std::string name, sf::Vector2i location);
    virtual ~Label();

  protected:
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui

#endif // GUILABEL_H
