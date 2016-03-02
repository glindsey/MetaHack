#ifndef GUILABEL_H
#define GUILABEL_H

#include <functional>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "gui/GUIObject.h"

class GUILabel :
  public GUIObject
{
public:
  GUILabel(std::string name, sf::Vector2i location);
  virtual ~GUILabel();

  virtual EventResult handle_event(sf::Event& event);

protected:
  virtual bool _render_self(sf::RenderTexture& texture, int frame) override final;

private:
};

#endif // GUILABEL_H
