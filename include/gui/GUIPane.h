#ifndef GUIPANE_H
#define GUIPANE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "gui/GUIObject.h"

class GUIPane :
  public GUIObject
{
public:
  GUIPane(sf::IntRect dimensions);
  virtual ~GUIPane();

  virtual EventResult handle_event(sf::Event& event);

protected:
  virtual bool _render_self(sf::RenderTarget& target, int frame) final;
  virtual std::string _render_contents(sf::RenderTarget& target, int frame);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // GUIPANE_H
