#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "EventHandler.h"
#include "Renderable.h"

/// Virtual superclass of all GUI objects on screen.
class GUIObject :
  public EventHandler,
  public Renderable
{
public:
  GUIObject(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
  GUIObject(std::string name, sf::IntRect dimensions);
  virtual ~GUIObject();

  std::string get_name();

  void set_focus(bool focus);
  bool get_focus();

  void set_text(std::string text);
  std::string get_text();

  sf::Vector2i get_relative_location();
  void set_relative_location(sf::Vector2i location);

  sf::Vector2i get_size();
  void set_size(sf::Vector2i size);

  sf::IntRect get_relative_dimensions();
  void set_relative_dimensions(sf::IntRect dimensions);

  sf::Vector2i get_absolute_location();
  void set_absolute_location(sf::Vector2i location);

  /// Add a child GUIObject underneath this one.
  /// This GUIObject assumes ownership of the child.
  /// @param child  Pointer to child to add.
  /// @return True if the object was added, false if the object was already a child of this GUIObject.
  bool add_child(std::unique_ptr<GUIObject> child);

  bool child_exists(std::string name);

  GUIObject& get_child(std::string name);

  template<typename ...args>
  void visit_children(std::function<void(GUIObject&, args...)> functor)
  {
    for (auto& child : children)
    {
      functor(*(child.get()), args...);
    }
  }

  /// Clear all children from this GUIObject.
  void clear_children();

  virtual EventResult handle_event(sf::Event& event) = 0;

  bool render(sf::RenderTarget& target, int frame);

protected:
  void set_parent(GUIObject* parent);

  virtual bool _render_self(sf::RenderTexture& texture, int frame) = 0;

private:
  /// The name of this object.
  std::string m_name;

  /// The parent of this object. Set to nullptr if the object has no parent.
  GUIObject* m_parent;

  /// Boolean indicating whether this object has the focus.
  bool m_focus;

  /// The text for this object. The way this text is used is dependent on the
  /// sort of control it is; e.g. for a Pane this is the pane title, for a
  /// Button it is the button caption, for a TextBox it is the box contents,
  /// etc.
  std::string m_text;

  /// Object location, relative to parent.
  sf::Vector2i m_location;

  /// Object size.
  sf::Vector2i m_size;

  /// Background texture.
  std::unique_ptr<sf::RenderTexture> m_bg_texture;

  /// Background shape.
  sf::RectangleShape m_bg_shape;

  /// Pointer vector of child elements.
  std::vector< std::unique_ptr<GUIObject> > m_children;
};

#endif // GUIOBJECT_H
