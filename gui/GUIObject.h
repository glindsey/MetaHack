#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include "stdafx.h"

#include "GUIEvent.h"
#include "types/ISFMLEventHandler.h"

#include "Observer.h"
#include "Subject.h"
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
  class Object : public Subject, public Observer
  {
  public:
    explicit Object(std::string name, IntVec2 location = IntVec2(0, 0), UintVec2 size = UintVec2(0, 0));
    Object(std::string name, sf::IntRect dimensions);
    virtual ~Object();

    std::string getName();

    /// Set whether this object has focus.
    /// When set to "true", will also unfocus any sibling controls.
    /// If the object is hidden or disabled, this method will do nothing.
    /// @note This only sets whether the object has focus in the context of its
    ///       parent. If the parent does not have focus, this object will not
    ///       be focused in the GUI until it does.
    void setFocus(bool focus);

    /// Get whether this object has focus.
    bool getFocus();

    /// Get whether this object has global focus (i.e., the entire chain of
    /// its parents also have focus).
    bool getGlobalFocus();

    /// Set whether this object has global focus.
    /// If the object is hidden or disabled, this method will do nothing.
    /// @param  focus   Whether this object has focus or not.
    void setGlobalFocus(bool focus);

    void setText(std::string text);
    std::string getText();

    /// Get location relative to parent object's client area.
    IntVec2 getRelativeLocation();

    /// Set location relative to parent object's client area.
    void setRelativeLocation(IntVec2 location);

    UintVec2 getSize();
    void setSize(UintVec2 size);

    sf::IntRect getRelativeDimensions();
    void setRelativeDimensions(sf::IntRect dimensions);

    // Get absolute location relative to root object.
    IntVec2 getAbsoluteLocation();
    void setAbsoluteLocation(IntVec2 location);

    sf::IntRect getAbsoluteDimensions();

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// @param child    std::unique_ptr to child to add.
    /// @param z_order  Z-order to put this child at. If omitted, uses the
    ///                 highest Z-order currently in the map, plus one.
    /// @return A pointer to the child added.
    Object* addChild(std::unique_ptr<Object> child,
                     uint32_t z_order);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A pointer to the child added.
    Object* addChild(std::unique_ptr<Object> child);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// @param child    Pointer to child to add.
    /// @param z_order  Z-order to put this child at. If omitted, uses the
    ///                 highest Z-order currently in the map, plus one.
    /// @return A pointer to the child added.
    template< typename T >
    T* addChild(T* child, uint32_t z_order)
    {
      std::unique_ptr<Object> child_ptr(child);
      return dynamic_cast<T*>(addChild(std::move(child_ptr), z_order));
    }

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    Pointer to child to add.
    ///
    /// @return A pointer to the child added.
    template< typename T >
    T* addChild(T* child)
    {
      std::unique_ptr<Object> child_ptr(child);
      return dynamic_cast<T*>(addChild(std::move(child_ptr)));
    }

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the lowest Z-order currently in
    /// the child map, minus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A pointer to the child added.
    Object* addChildTop(std::unique_ptr<Object> child);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the lowest Z-order currently in
    /// the child map, minus one.
    /// @param child    Pointer to child to add.
    ///
    /// @return A reference to the child added.
    template< typename T >
    T* addChildTop(T* child)
    {
      std::unique_ptr<Object> child_ptr(child);
      return dynamic_cast<T*>(addChildTop(std::move(child_ptr)));
    }

    /// Handle an incoming event.
    /// Calls the method handleGUIEventPreChildren() first.
    /// If it returns anything other than Handled, it then passes the
    /// event down to each child in sequence, stopping only if one of them
    /// returns Handled. If the event still isn't Handled after all children
    /// have seen it, handle_event_after_children_ is called.
    template< typename T >
    GUIEvent::Result handleGUIEvent(T& event)
    {
      GUIEvent::Result result = GUIEvent::Result::Pending;

      if (m_cached_flags.disabled == false)
      {
        // Check the event for us first.
        result = handleGUIEventPreChildren(event);

        // Possible results here:
        //   * The event doesn't apply to us at all. (Ignored)
        //        Do not pass to children, and do not call handle_event_after_children.
        //   * The event may apply to us or to our children (Acknowledged)
        //        Pass to children.
        //        As long as no child responded "Handled", call handle_event_after_children.
        //   * The event applies to us, and we handled it. (Handled)
        //        Do not pass to children, and do not call handle_event_after_children.

        if ((result != GUIEvent::Result::Ignored) &&
          (result != GUIEvent::Result::Handled))
        {
          for (auto& z_pair : m_zorder_map)
          {
            GUIEvent::Result child_result = GUIEvent::Result::Acknowledged;
            auto& child = m_children.at(z_pair.second);
            child_result = child->handleGUIEvent(event);
            if (child_result == GUIEvent::Result::Handled)
            {
              result = GUIEvent::Result::Handled;
              break;
            }
          }
        }

        if ((result != GUIEvent::Result::Ignored) &&
          (result != GUIEvent::Result::Handled))
        {
          result = handleGUIEventPostChildren(event);
        }
      }

      return result;
    }

    bool childExists(std::string name);

    Object& getChild(std::string name);

    /// Remove the child object with the given name, if it exists.
    /// @param name   Name of the child object to remove.
    /// @return Pointer to the removed object if it existed,
    ///         empty unique_ptr otherwise.
    std::unique_ptr<Object> removeChild(std::string name);

    /// Get lowest Z-order of all this object's children.
    /// If no children are present, returns zero.
    uint32_t getLowestChildZOrder();

    /// Get highest Z-order of all this object's children.
    /// If no children are present, returns zero.
    uint32_t getHighestChildZOrder();

    template< typename ...args >
    void visitChildren(std::function<void(Object&, args...)> functor)
    {
      for (auto& child_pair : children)
      {
        functor(*(child_pair.second), args...);
      }
    }

    /// Clear all children from this GUIObject.
    void clearChildren();

    /// Get the upper-left corner of this object's child area, relative to
    /// its own upper-left corner.
    /// By default, the child area encompasses the entire object, so this
    /// returns (0, 0); however, subclasses can override this behavior.
    virtual IntVec2 getChildAreaLocation();

    /// Get the size of this object's child area.
    /// By default, the child area encompasses the entire object, so this
    /// returns getSize(); however, subclasses can override this behavior.
    virtual UintVec2 getChildAreaSize();

    /// Render this object, and all of its children, to the parent texture.
    bool render(sf::RenderTexture& texture, int frame);

    /// Set/clear an object flag.
    /// Calls the virtual method handle_set_flag_ if the flag has been
    /// changed; this allows subclasses to perform specific actions based on
    /// certain flags (such as setting/clearing "titlebar").
    void setFlag(std::string name, bool enabled);

    /// Get an object flag.
    /// A flag that does not exist will be initialized and set to false or
    /// the default value given.
    bool getFlag(std::string name, bool default_value = false);

    /// Handles a flag being set/cleared.
    /// If this function does not handle a particular flag, calls the
    /// virtual function handleSetFlag_().
    void handleSetFlag(std::string name, bool enabled);

    /// Returns whether the specified point falls within this object's bounds.
    /// @param  point   Point to check.
    /// @return True if the point is within the object, false otherwise.
    bool containsPoint(IntVec2 point);

    /// Flag this object, and its parents, to be redrawn.
    void flagForRedraw();

    GUIEvent::Result handleGUIEventPreChildren(GUIEventDragFinished& event);
    GUIEvent::Result handleGUIEventPostChildren(GUIEventDragFinished& event);

    GUIEvent::Result handleGUIEventPreChildren(GUIEventDragStarted& event);
    GUIEvent::Result handleGUIEventPostChildren(GUIEventDragStarted& event);

    GUIEvent::Result handleGUIEventPreChildren(GUIEventDragging& event);
    GUIEvent::Result handleGUIEventPostChildren(GUIEventDragging& event);

    GUIEvent::Result handleGUIEventPreChildren(GUIEventKeyPressed& event);
    GUIEvent::Result handleGUIEventPostChildren(GUIEventKeyPressed& event);

    GUIEvent::Result handleGUIEventPreChildren(GUIEventMouseDown& event);
    GUIEvent::Result handleGUIEventPostChildren(GUIEventMouseDown& event);

    GUIEvent::Result handleGUIEventPreChildren(GUIEventResized& event);
    GUIEvent::Result handleGUIEventPostChildren(GUIEventResized& event);

    virtual std::unordered_set<EventID> registeredEvents() const override;

  protected:
    Object* getParent();

    void setParent(Object* parent);

    /// Redraw this object on its own background texture.
    void draw(int frame);

    /// Clear the focus of all of this object's children.
    void clearChildFocuses();

    /// Set the focus of an object without clearing sibling focuses (foci?).
    void setFocusOnly(bool focus);

    /// Returns whether this object is currently being dragged.
    bool isBeingDragged();

    /// Returns the location of the start of the last drag.
    IntVec2 getDragStartLocation();

    /// Called before rendering the object's children.
    /// Default behavior is to do nothing.
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame);

    /// Called after rendering the object's children.
    /// Default behavior is to do nothing.
    virtual void drawPostChildren_(sf::RenderTexture& texture, int frame);

    virtual GUIEvent::Result handleGUIEventPreChildren_(GUIEventDragFinished& event);
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventDragFinished& event);

    virtual GUIEvent::Result handleGUIEventPreChildren_(GUIEventDragStarted& event);
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventDragStarted& event);

    virtual GUIEvent::Result handleGUIEventPreChildren_(GUIEventDragging& event);
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventDragging& event);

    virtual GUIEvent::Result handleGUIEventPreChildren_(GUIEventKeyPressed& event);
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventKeyPressed& event);

    virtual GUIEvent::Result handleGUIEventPreChildren_(GUIEventMouseDown& event);
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventMouseDown& event);

    virtual GUIEvent::Result handleGUIEventPreChildren_(GUIEventResized& event);
    virtual GUIEvent::Result handleGUIEventPostChildren_(GUIEventResized& event);

    /// Handles a flag being set/cleared.
    /// This method is called by setFlag() if the value was changed.
    /// The default behavior is to do nothing.
    virtual void handleSetFlag_(std::string name, bool enabled);

    /// Handles the parent's size being changed.
    /// The default behavior is to do nothing.
    virtual void handleParentSizeChanged_(UintVec2 parent_size);

    virtual EventResult onEvent_(Event const& event);

    /// Subscribe to parent events that all objects care about.
    void subscribeToParentEvents(Subject const& parent, int priority);

    /// Subscribe to any additional events that we care about which are 
    /// emitted by a parent.
    /// The default behavior is to do nothing.
    virtual void subscribeToParentEvents_(Subject const& parent, int priority);


  private:
    /// The name of this object.
    std::string m_name;

    /// The parent of this object. Set to nullptr if the object has no parent.
    Object* m_parent;

    /// Boolean indicating whether this object has the focus.
    /// Focus is handled differently and NOT put into m_flags because of
    /// how it propogates along the object tree.
    bool m_focus = false;

    /// Flag indicating whether this object needs to be redrawn.
    bool m_flag_for_redraw = true;

    /// Definition of struct of cached flag values.
    struct CachedFlags
    {
      bool hidden = false;     ///< Means this object, and its children, are not displayed.
      bool disabled = false;   ///< Means this object does not process events.
      bool animated = false;   ///< Means this object must be redrawn every frame.
      bool movable = false;    ///< Means this object can be moved via dragging.
      bool decor = false;      ///< Means this object is part of its parent's decor.
    };

    /// Struct of cached flag values.
    CachedFlags m_cached_flags;

    /// Boolean indicating whether a drag is currently in progress.
    bool m_being_dragged = false;

    /// The location that the last drag started.
    IntVec2 m_drag_start_location;

    /// The text for this object. The way this text is used is dependent on the
    /// sort of control it is; e.g. for a Pane this is the pane title, for a
    /// Button it is the button caption, for a TextBox it is the box contents,
    /// etc.
    std::string m_text;

    /// Object location, relative to parent.
    IntVec2 m_location;

    /// Location as captured at last mousedown.
    IntVec2 m_absolute_location_drag_start;

    /// Object size.
    UintVec2 m_size;

    /// Background texture.
    std::unique_ptr<sf::RenderTexture> m_bg_texture;

    /// Size of the background texture.
    /// Should be equal to the next largest power of 2 after m_size.
    UintVec2 m_bg_texture_size;

    /// Background shape.
    sf::RectangleShape m_bg_shape;

    /// Map of flags that can be set/cleared for this object.
    BoolMap m_flags;

    /// Map that owns the child elements.
    std::unordered_map< std::string, std::unique_ptr<Object> > m_children;

    /// Multimap that associates child elements with Z-orders.
    std::multimap< uint32_t, std::string > m_zorder_map;
  };

  /// Convenience function for calculating the distance between two
  /// IntVec2 points.
  inline unsigned int distance(IntVec2 first, IntVec2 second)
  {
    int x_distance = first.x - second.x;
    int y_distance = first.y - second.y;
    return static_cast<unsigned int>(sqrt((x_distance * x_distance) + (y_distance * y_distance)));
  }
}; // end namespace metagui

#endif // GUIOBJECT_H
