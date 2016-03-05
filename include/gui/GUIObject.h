#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include "stdafx.h"

#include "EventHandler.h"
#include "Renderable.h"

namespace metagui
{
  /// Forward declarations
  class Object;

  /// Using declarations
  using ChildMap = std::map< std::string, std::unique_ptr<Object> >;
  using ZOrderMap = std::multimap< uint32_t, std::string >;

  /// Virtual superclass of all GUI objects on screen.
  /// @todo Should child objects store Z-order?
  class Object :
    public EventHandler,
    public Renderable
  {
  public:
    explicit Object(std::string name, sf::Vector2i location = sf::Vector2i(0, 0), sf::Vector2i size = sf::Vector2i(0, 0));
    Object(std::string name, sf::IntRect dimensions);
    virtual ~Object();

    std::string get_name();

    /// Set whether this object has focus.
    /// When set to "true", will also unfocus any sibling controls.
    /// @note This only sets whether the object has focus in the context of its
    ///       parent. If the parent does not have focus, this object will not
    ///       be focused in the GUI until it does.
    void set_focus(bool focus);

    /// Get whether this object has focus.
    bool get_focus();

    /// Get whether this object has global focus (i.e., the entire chain of
    /// its parents also have focus).
    bool get_global_focus();

    /// Set whether this object has global focus.
    void set_global_focus(bool focus);

    /// Set whether this object is hidden.
    /// A hidden object will not be rendered to the screen, nor will any of
    /// its children.
    void set_hidden(bool hidden);

    /// Get whether this object is hidden.
    bool get_hidden();

    /// Set whether this object is enabled.
    /// A disabled object will not have events passed to it, nor to its children.
    /// Depending on the object's render() method, it may be rendered differently
    /// when disabled.
    void set_enabled(bool enabled);

    /// Get whether thsi object is disabled.
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
    /// *This GUIObject assumes ownership of the child.*
    /// @param child    std::unique_ptr to child to add.
    /// @param z_order  Z-order to put this child at. If omitted, uses the
    ///                 highest Z-order currently in the map, plus one.
    /// @return A reference to the child added.
    Object& add_child(std::unique_ptr<Object> child,
                      uint32_t z_order);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// @param child    Pointer to child to add.
    /// @param z_order  Z-order to put this child at. If omitted, uses the
    ///                 highest Z-order currently in the map, plus one.
    /// @return A reference to the child added.
    Object& add_child(Object* child,
                      uint32_t z_order);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A reference to the child added.
    Object& add_child(std::unique_ptr<Object> child);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    Pointer to child to add.
    ///
    /// @return A reference to the child added.
    Object& add_child(Object* child);


    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the lowest Z-order currently in
    /// the child map, minus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A reference to the child added.
    Object& add_child_top(std::unique_ptr<Object> child);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the lowest Z-order currently in
    /// the child map, minus one.
    /// @param child    Pointer to child to add.
    ///
    /// @return A reference to the child added.
    Object& add_child_top(Object* child);

    bool child_exists(std::string name);

    Object& get_child(std::string name);

    /// Remove the child object with the given name, if it exists.
    /// @param name   Name of the child object to remove.
    /// @return Pointer to the removed object if it existed,
    ///         empty unique_ptr otherwise.
    std::unique_ptr<Object> remove_child(std::string name);

    /// Get lowest Z-order of all this object's children.
    /// If no children are present, returns zero.
    uint32_t get_lowest_child_z_order();

    /// Get highest Z-order of all this object's children.
    /// If no children are present, returns zero.
    uint32_t get_highest_child_z_order();

    template<typename ...args>
    void visit_children(std::function<void(Object&, args...)> functor)
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

    /// Render this object, and all of its children, to the screen.
    bool render(sf::RenderTarget& target, int frame);

    /// Handle an incoming event.
    /// Calls the virtual function handle_event_before_children_ first.
    /// If it returns anything other than Handled, it then passes the
    /// event down to each child in sequence, stopping only if one of them
    /// returns Handled. If the event still isn't Handled after all children
    /// have seen it, handle_event_after_children_ is called.
    EventResult handle_event(sf::Event& event);

  protected:
    void set_parent(Object* parent);

    /// Clear the focus of all of this object's children.
    void clear_child_focuses();

    /// Set the focus of an object without clearing sibling focuses (foci?).
    void set_focus_only(bool focus);

    /// Called before rendering the object's children.
    /// Default behavior is to do nothing.
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame);

    /// Called after rendering the object's children.
    /// Default behavior is to do nothing.
    virtual void render_self_after_children_(sf::RenderTexture& texture, int frame);

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

  private:
    /// The name of this object.
    std::string m_name;

    /// The parent of this object. Set to nullptr if the object has no parent.
    Object* m_parent;

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

    /// Map that owns the child elements.
    std::map< std::string, std::unique_ptr<Object> > m_children;

    /// Multimap that associates child elements with Z-orders.
    std::map< uint32_t, std::string > m_zorder_map;
  };
}; // end namespace metagui

#endif // GUIOBJECT_H
