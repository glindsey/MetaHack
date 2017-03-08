#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include "stdafx.h"

#include "GUIEvent.h"
#include "Renderable.h"
#include "SFMLEventHandler.h"

#include "Visitor.h"

namespace metagui
{
  // Forward declaration of Object class
  class Object;

  // Forward declaration of any class that can be visited
  class CloseHandle;
  class Desktop;
  class Label;
  class ResizeHandle;
  class ShrinkHandle;
  class TitleBar;
  class Window;

  /// Using declarations
  using ChildMap = std::map< std::string, std::unique_ptr<Object> >;
  using ZOrderMap = std::multimap< uint32_t, std::string >;
  using RenderFunctor = std::function< void(sf::RenderTexture&, int) >;

  // The following declaration should include every possible GUIObject that
  // we want to be able to accept visitors.
  using ObjectVisitor = Visitor<
    CloseHandle,
    Desktop,
    Label,
    ResizeHandle,
    ShrinkHandle,
    TitleBar,
    Window
  >;

  // The following declaration should have the same class list as above.
  template<class T>
  using ObjectVisitable = VisitableImpl<T,
    CloseHandle,
    Desktop,
    Label,
    ResizeHandle,
    ShrinkHandle,
    TitleBar,
    Window
  >;

  /// Virtual superclass of all GUI objects on screen.
  /// @todo Should child objects store Z-order?
  class Object :
    public RenderableToTexture
  {
  public:
    explicit Object(std::string name, Vec2i location = Vec2i(0, 0), Vec2u size = Vec2u(0, 0));
    Object(std::string name, sf::IntRect dimensions);
    virtual ~Object();

    std::string get_name();

    /// Set pre-child render functor.
    /// If present, this functor is called after render_self_before_children_()
    /// is executed, but before children are rendered.
    /// @param  functor   Functor to call.
    void set_pre_child_render_functor(RenderFunctor functor);

    /// Clear the pre-child render functor.
    void clear_pre_child_render_functor();

    /// Set post-child render functor.
    /// If present, this functor is called after children are rendered,
    /// but before render_self_after_children_() is executed.
    /// @param  functor   Functor to call.
    void set_post_child_render_functor(RenderFunctor functor);

    /// Clear the pre-child render functor.
    void clear_post_child_render_functor();

    /// Set whether this object has focus.
    /// When set to "true", will also unfocus any sibling controls.
    /// If the object is hidden or disabled, this method will do nothing.
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
    /// If the object is hidden or disabled, this method will do nothing.
    /// @param  focus   Whether this object has focus or not.
    void set_global_focus(bool focus);

    void set_text(std::string text);
    std::string get_text();

    /// Get location relative to parent object's client area.
    Vec2i get_relative_location();

    /// Set location relative to parent object's client area.
    void set_relative_location(Vec2i location);

    Vec2u get_size();
    void set_size(Vec2u size);

    sf::IntRect get_relative_dimensions();
    void set_relative_dimensions(sf::IntRect dimensions);

    // Get absolute location relative to root object.
    Vec2i get_absolute_location();
    void set_absolute_location(Vec2i location);

    sf::IntRect get_absolute_dimensions();

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
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A reference to the child added.
    Object& add_child(std::unique_ptr<Object> child);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// @param child    Pointer to child to add.
    /// @param z_order  Z-order to put this child at. If omitted, uses the
    ///                 highest Z-order currently in the map, plus one.
    /// @return A reference to the child added.
    template< typename T >
    T& add_child(T* child, uint32_t z_order)
    {
      std::unique_ptr<Object> child_ptr(child);
      return dynamic_cast<T&>(add_child(std::move(child_ptr), z_order));
    }

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    Pointer to child to add.
    ///
    /// @return A reference to the child added.
    template< typename T >
    T& add_child(T* child)
    {
      std::unique_ptr<Object> child_ptr(child);
      return dynamic_cast<T&>(add_child(std::move(child_ptr)));
    }

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
    template< typename T >
    T& add_child_top(T* child)
    {
      std::unique_ptr<Object> child_ptr(child);
      return dynamic_cast<T&>(add_child_top(std::move(child_ptr)));
    }

    /// Handle an incoming event.
    /// Calls the method handle_event_before_children() first.
    /// If it returns anything other than Handled, it then passes the
    /// event down to each child in sequence, stopping only if one of them
    /// returns Handled. If the event still isn't Handled after all children
    /// have seen it, handle_event_after_children_ is called.
    template< typename T >
    Event::Result handle_gui_event(T& event)
    {
      Event::Result result = Event::Result::Pending;

      if (m_cached_flags.disabled == false)
      {
        // Check the event for us first.
        result = handle_event_before_children(event);

        // Possible results here:
        //   * The event doesn't apply to us at all. (Ignored)
        //        Do not pass to children, and do not call handle_event_after_children.
        //   * The event may apply to us or to our children (Acknowledged)
        //        Pass to children.
        //        As long as no child responded "Handled", call handle_event_after_children.
        //   * The event applies to us, and we handled it. (Handled)
        //        Do not pass to children, and do not call handle_event_after_children.

        if ((result != Event::Result::Ignored) &&
          (result != Event::Result::Handled))
        {
          for (auto& z_pair : m_zorder_map)
          {
            Event::Result child_result = Event::Result::Acknowledged;
            auto& child = m_children.at(z_pair.second);
            child_result = child->handle_gui_event(event);
            if (child_result == Event::Result::Handled)
            {
              result = Event::Result::Handled;
              break;
            }
          }
        }

        if ((result != Event::Result::Ignored) &&
          (result != Event::Result::Handled))
        {
          result = handle_event_after_children(event);
        }
      }

      return result;
    }

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

    template< typename ...args >
    void visit_children(std::function<void(Object&, args...)> functor)
    {
      for (auto& child_pair : children)
      {
        functor(*(child_pair.second), args...);
      }
    }

    /// Clear all children from this GUIObject.
    void clear_children();

    /// Get the upper-left corner of this object's child area, relative to
    /// its own upper-left corner.
    /// By default, the child area encompasses the entire object, so this
    /// returns (0, 0); however, subclasses can override this behavior.
    virtual Vec2i get_child_area_location();

    /// Get the size of this object's child area.
    /// By default, the child area encompasses the entire object, so this
    /// returns get_size(); however, subclasses can override this behavior.
    virtual Vec2u get_child_area_size();

    /// Render this object, and all of its children, to the parent texture.
    bool render(sf::RenderTexture& texture, int frame);

    /// Set/clear an object flag.
    /// Calls the virtual method handle_set_flag_ if the flag has been
    /// changed; this allows subclasses to perform specific actions based on
    /// certain flags (such as setting/clearing "titlebar").
    void set_flag(std::string name, bool enabled);

    /// Get an object flag.
    /// A flag that does not exist will be initialized and set to false or
    /// the default value given.
    bool get_flag(std::string name, bool default_value = false);

    /// Handles a flag being set/cleared.
    /// If this function does not handle a particular flag, calls the
    /// virtual function handle_set_flag_().
    void handle_set_flag(std::string name, bool enabled);

    /// Returns whether the specified point falls within this object's bounds.
    /// @param  point   Point to check.
    /// @return True if the point is within the object, false otherwise.
    bool contains_point(Vec2i point);

    Event::Result handle_event_before_children(EventDragFinished& event);
    Event::Result handle_event_after_children(EventDragFinished& event);

    Event::Result handle_event_before_children(EventDragStarted& event);
    Event::Result handle_event_after_children(EventDragStarted& event);

    Event::Result handle_event_before_children(EventDragging& event);
    Event::Result handle_event_after_children(EventDragging& event);

    Event::Result handle_event_before_children(EventKeyPressed& event);
    Event::Result handle_event_after_children(EventKeyPressed& event);

    Event::Result handle_event_before_children(EventMouseDown& event);
    Event::Result handle_event_after_children(EventMouseDown& event);

    Event::Result handle_event_before_children(EventResized& event);
    Event::Result handle_event_after_children(EventResized& event);

  protected:
    Object* get_parent();

    void set_parent(Object* parent);

    /// Clear the focus of all of this object's children.
    void clear_child_focuses();

    /// Set the focus of an object without clearing sibling focuses (foci?).
    void set_focus_only(bool focus);

    /// Returns whether this object is currently being dragged.
    bool is_being_dragged();

    /// Returns the location of the start of the last drag.
    Vec2i get_drag_start_location();

    /// Called before rendering the object's children.
    /// Default behavior is to do nothing.
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame);

    /// Called after rendering the object's children.
    /// Default behavior is to do nothing.
    virtual void render_self_after_children_(sf::RenderTexture& texture, int frame);

    virtual Event::Result handle_event_before_children_(EventDragFinished& event);
    virtual Event::Result handle_event_after_children_(EventDragFinished& event);

    virtual Event::Result handle_event_before_children_(EventDragStarted& event);
    virtual Event::Result handle_event_after_children_(EventDragStarted& event);

    virtual Event::Result handle_event_before_children_(EventDragging& event);
    virtual Event::Result handle_event_after_children_(EventDragging& event);

    virtual Event::Result handle_event_before_children_(EventKeyPressed& event);
    virtual Event::Result handle_event_after_children_(EventKeyPressed& event);

    virtual Event::Result handle_event_before_children_(EventMouseDown& event);
    virtual Event::Result handle_event_after_children_(EventMouseDown& event);

    virtual Event::Result handle_event_before_children_(EventResized& event);
    virtual Event::Result handle_event_after_children_(EventResized& event);

    /// Handles a flag being set/cleared.
    /// This method is called by set_flag() if the value was changed.
    /// The default behavior is to do nothing.
    virtual void handle_set_flag_(std::string name, bool enabled);

    /// Handles the parent's size being changed.
    /// The default behavior is to do nothing.
    virtual void handle_parent_size_changed_(Vec2u parent_size);


  private:
    /// The name of this object.
    std::string m_name;

    /// The parent of this object. Set to nullptr if the object has no parent.
    Object* m_parent;

    /// Boolean indicating whether this object has the focus.
    /// Focus is handled differently and NOT put into m_flags because of
    /// how it propogates along the object tree.
    bool m_focus = false;

    /// Definition of struct of cached flag values.
    struct CachedFlags
    {
      bool hidden = false;
      bool disabled = false;
      bool movable = false;
      bool decor = false;
    };

    /// Struct of cached flag values.
    CachedFlags m_cached_flags;

    /// Boolean indicating whether a drag is currently in progress.
    bool m_being_dragged = false;

    /// The location that the last drag started.
    Vec2i m_drag_start_location;

    /// The text for this object. The way this text is used is dependent on the
    /// sort of control it is; e.g. for a Pane this is the pane title, for a
    /// Button it is the button caption, for a TextBox it is the box contents,
    /// etc.
    std::string m_text;

    /// Object location, relative to parent.
    Vec2i m_location;

    /// Location as captured at last mousedown.
    Vec2i m_absolute_location_drag_start;

    /// Object size.
    Vec2u m_size;

    /// Background texture.
    std::unique_ptr<sf::RenderTexture> m_bg_texture;

    /// Background shape.
    sf::RectangleShape m_bg_shape;

    /// Pre-child render functor.
    RenderFunctor m_pre_child_render_functor;

    /// Post-child render functor.
    RenderFunctor m_post_child_render_functor;

    /// Map of flags that can be set/cleared for this object.
    BoolMap m_flags;

    /// Map that owns the child elements.
    std::unordered_map< std::string, std::unique_ptr<Object> > m_children;

    /// Multimap that associates child elements with Z-orders.
    std::multimap< uint32_t, std::string > m_zorder_map;
  };

  /// Convenience function for calculating the distance between two
  /// Vec2i points.
  inline unsigned int distance(Vec2i first, Vec2i second)
  {
    int x_distance = first.x - second.x;
    int y_distance = first.y - second.y;
    return static_cast<unsigned int>(sqrt((x_distance * x_distance) + (y_distance * y_distance)));
  }
}; // end namespace metagui

#endif // GUIOBJECT_H
