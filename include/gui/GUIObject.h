#ifndef GUIOBJECT_H
#define GUIOJBECT_H

#include <memory>

#include "EventHandler.h"
#include "Renderable.h"

/// Virtual superclass of all GUI objects on screen.
class GUIObject :
  public EventHandler,
  public Renderable
{
public:
  GUIObject(sf::IntRect dimensions);
  virtual ~GUIObject();

  void set_focus(bool focus);
  bool get_focus();

  sf::IntRect get_dimensions();
  void set_dimensions(sf::IntRect dimensions);

  virtual EventResult handle_event(sf::Event& event) = 0;

  bool render(sf::RenderTarget& target, int frame);

protected:
  virtual bool _render_self(sf::RenderTarget& target, int frame) = 0;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // GUIOBJECT_H
