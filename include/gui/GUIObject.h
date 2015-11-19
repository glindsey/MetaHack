#ifndef GUIOBJECT_H
#define GUIOBJECT_H

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

  /// Add a child GUIObject underneath this one.
  /// This GUIObject assumes ownership of the child.
  /// @param child  Reference to child to add.
  /// @return True if the object was added, false if the object was already a child of this GUIObject.
  bool add_child(GUIObject& child);

  void clear_children();

  virtual EventResult handle_event(sf::Event& event) = 0;

  bool render(sf::RenderTarget& target, int frame);

protected:
  virtual bool _render_self(sf::RenderTarget& target, int frame) = 0;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // GUIOBJECT_H
