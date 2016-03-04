#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include "stdafx.h"

#include "EventHandler.h"
#include "Renderable.h"

/// Forward declarations
class GUIObject;

/// Using declarations
using ChildMap = std::multimap< uint32_t, std::unique_ptr<GUIObject> >;
using ChildPair = std::pair< uint32_t, std::unique_ptr<GUIObject> >;

/// Virtual superclass of all GUI objects on screen.
/// @todo Should child objects store Z-order?
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

  void set_hidden(bool hidden);
  bool get_hidden();

  void set_enabled(bool enabled);
  bool get_enabled();

  void set_text(std::string text);
  std::string get_text();

  /// Get location relative to parent object's client area.
  sf::Vector2i get_relative_location();

  /// Set location relative to parent object's client area.
  void set_relative_location(sf::Vector2i location);

  sf::Vector2i get_size();
  void set_size(sf::Vector2i size);

  sf::IntRect get_relative_dimensions();
  void set_relative_dimensions(sf::IntRect dimensions);

  // Get absolute location relative to root object.
  sf::Vector2i get_absolute_location();

  void set_absolute_location(sf::Vector2i location);

  /// Add a child GUIObject underneath this one.
  /// This GUIObject assumes ownership of the child.
  /// @param child    std::unique_ptr to child to add.
  /// @param z_order  Z-order to put this child at. If omitted, uses the
  ///                 highest Z-order currently in the map, plus one.
  /// @return A reference to the child added.
  GUIObject& add_child(std::unique_ptr<GUIObject> child, 
                       uint32_t z_order);

  /// Add a child GUIObject underneath this one.
  /// This GUIObject assumes ownership of the child.
  /// The new child's Z-order will be set to the highest Z-order currently in 
  /// the child map, plus one.
  /// @param child    std::unique_ptr to child to add.
  ///                
  /// @return A reference to the child added.
  GUIObject& add_child(std::unique_ptr<GUIObject> child);

  /// Add a child GUIObject underneath this one.
  /// This GUIObject assumes ownership of the child.
  /// The new child's Z-order will be set to the lowest Z-order currently in 
  /// the child map, minus one.
  /// @param child    std::unique_ptr to child to add.
  ///                
  /// @return A reference to the child added.
  GUIObject& add_child_top(std::unique_ptr<GUIObject> child);

  bool child_exists(std::string name);

  GUIObject& get_child(std::string name);

  /// Get lowest Z-order of all this object's children.
  /// If no children are present, returns zero.
  uint32_t get_lowest_child_z_order();

  /// Get highest Z-order of all this object's children.
  /// If no children are present, returns zero.
  uint32_t get_highest_child_z_order();

  template<typename ...args>
  void visit_children(std::function<void(GUIObject&, args...)> functor)
  {
    for (auto& child_pair : children)
    {
      functor(*((child_pair.second).get()), args...);
    }
  }

  /// Clear all children from this GUIObject.
  void clear_children();

  /// Get the upper-left corner of this object's child area, relative to
  /// its own upper-left corner.
  /// By default, the child area encompasses the entire object, so this
  /// returns (0, 0); however, subclasses can override this behavior.
  virtual sf::Vector2i get_child_area_location();

  /// Get the size of this object's child area.
  /// By default, the child area encompasses the entire object, so this
  /// returns get_size(); however, subclasses can override this behavior.
  virtual sf::Vector2i get_child_area_size();

  bool render(sf::RenderTarget& target, int frame);

  /// Handle an incoming event.
  /// Calls the virtual function handle_event_before_children_ first.
  /// If it returns anything other than Handled, it then passes the
  /// event down to each child in sequence, stopping only if one of them
  /// returns Handled. If the event still isn't Handled after all children
  /// have seen it, handle_event_after_children_ is called.
  EventResult handle_event(sf::Event& event);

protected:
  void set_parent(GUIObject* parent);

  /// Called before an event is passed along to child objects.
  /// Default behavior is to return EventResult::Ignored.
  /// @note If this method returns EventResult::Handled, event processing
  ///       will stop here and the children will never see the event! To
  ///       process it here *and* have children see it, you should return
  ///       EventResult::Acknowledged.
  virtual EventResult handle_event_before_children_(sf::Event& event);

  /// Called after an event is passed along to child objects.
  /// This method will only be called if none of the child objects returns
  /// EventResult::Handled when the event is passed to it.
  /// Default behavior is to return EventResult::Ignored.
  virtual EventResult handle_event_after_children_(sf::Event& event);

  virtual bool _render_self(sf::RenderTexture& texture, int frame) = 0;

private:
  /// The name of this object.
  std::string m_name;

  /// The parent of this object. Set to nullptr if the object has no parent.
  GUIObject* m_parent;

  /// Boolean indicating whether this object has the focus.
  bool m_focus = false;

  /// Boolean indicating whether this object is hidden.
  bool m_hidden = false;

  /// Boolean indicating whether this object is enabled.
  bool m_enabled = true;

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

  /// Pointer multimap associating Z-orders with child elements.
  ChildMap m_children;

  //std::vector< std::unique_ptr<GUIObject> > m_children;
};

#endif // GUIOBJECT_H
