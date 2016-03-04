#ifndef GUILABEL_H
#define GUILABEL_H

#include "stdafx.h"

#include "gui/GUIObject.h"

class GUILabel :
  public GUIObject
{
public:
  GUILabel(std::string name, sf::Vector2i location);
  virtual ~GUILabel();

protected:
  virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

private:
};

#endif // GUILABEL_H
