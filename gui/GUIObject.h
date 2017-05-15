#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include "stdafx.h"

#include "events/UIEvents.h"

#include "Object.h"
#include "Visitor.h"

namespace metagui
{
  // Forward declaration of Object class
  class GUIObject;

  // Forward declaration of any class that can be visited
  class CloseHandle;
  class Desktop;
  class Label;
  class ResizeHandle;
  class ShrinkHandle;
  class TitleBar;
  class Window;

  /// Using declarations
  using ChildMap = std::map< std::string, std::unique_ptr<GUIObject> >;
  using ZOrderMap = std::multimap< uint32_t, std::string >;
  using RenderFunctor = std::function< void(sf::RenderTexture&, int) >;

  // The following declaration should include every possible GUIObject that
  // we want to be able to accept visitors.
  using GUIObjectVisitor = Visitor<
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
  using GUIObjectVisitable = VisitableImpl<T,
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
  class GUIObject : public Object
  {
  public:

    struct EventMoved : public ConcreteEvent<EventMoved>
    {
      EventMoved(IntVec2 old_position_, IntVec2 new_position_)
        :
        old_position{ old_position_ },
        new_position{ new_position_ }
      {}

      IntVec2 const old_position;
      IntVec2 const new_position;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | old: " << old_position << " | new: " << new_position;
      }
    };


    struct EventResized : public ConcreteEvent<EventResized>
    {
      EventResized(UintVec2 old_size_, UintVec2 new_size_)
        :
        old_size{ old_size_ },
        new_size{ new_size_ }
      {}

      UintVec2 const old_size;
      UintVec2 const new_size;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | old: " << old_size << " | new: " << new_size;
      }
    };

    struct EventDragFinished : public ConcreteEvent<EventDragFinished>
    {
      EventDragFinished(sf::Mouse::Button button_, IntVec2 current_location_)
        :
        button(button_),
        current_location(current_location_)
      {}

      sf::Mouse::Button const button;
      IntVec2 const current_location;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | button: " << button << " | end: " << current_location;
      }
    };

    struct EventDragStarted : public ConcreteEvent<EventDragStarted>
    {
      EventDragStarted(sf::Mouse::Button button_, IntVec2 start_location_)
        :
        button(button_),
        start_location(start_location_)
      {}

      sf::Mouse::Button const button;
      IntVec2 const start_location;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | button: " << button << " | start: " << start_location;
      }
    };

    struct EventDragging : public ConcreteEvent<EventDragging>
    {
      EventDragging(sf::Mouse::Button button_, IntVec2 current_location_)
        :
        button(button_),
        current_location(current_location_)
      {}

      sf::Mouse::Button const button;
      IntVec2 const current_location;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | button: " << button << " | loc: " << current_location;
      }

      /// Number of pixels you have to move before it is considered "dragging" the object.
      static unsigned int const drag_threshold = 16;
    };

    explicit GUIObject(std::string name, IntVec2 location = IntVec2(0, 0), UintVec2 size = UintVec2(0, 0));
    GUIObject(std::string name, sf::IntRect dimensions);
    virtual ~GUIObject();

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
    GUIObject* addChild(std::unique_ptr<GUIObject> child,
                        uint32_t z_order);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the highest Z-order currently in
    /// the child map, plus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A pointer to the child added.
    GUIObject* addChild(std::unique_ptr<GUIObject> child);

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// @param child    Pointer to child to add.
    /// @param z_order  Z-order to put this child at. If omitted, uses the
    ///                 highest Z-order currently in the map, plus one.
    /// @return A pointer to the child added.
    template< typename T >
    T* addChild(T* child, uint32_t z_order)
    {
      std::unique_ptr<GUIObject> child_ptr(child);
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
      std::unique_ptr<GUIObject> child_ptr(child);
      return dynamic_cast<T*>(addChild(std::move(child_ptr)));
    }

    /// Add a child GUIObject underneath this one.
    /// *This GUIObject assumes ownership of the child.*
    /// The new child's Z-order will be set to the lowest Z-order currently in
    /// the child map, minus one.
    /// @param child    std::unique_ptr to child to add.
    ///
    /// @return A pointer to the child added.
    GUIObject* addChildTop(std::unique_ptr<GUIObject> child);

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
      std::unique_ptr<GUIObject> child_ptr(child);
      return dynamic_cast<T*>(addChildTop(std::move(child_ptr)));
    }

    bool childExists(std::string name);

    GUIObject& getChild(std::string name);

    /// Remove the child object with the given name, if it exists.
    /// @param name   Name of the child object to remove.
    /// @return Pointer to the removed object if it existed,
    ///         empty unique_ptr otherwise.
    std::unique_ptr<GUIObject> removeChild(std::string name);

    /// Get lowest Z-order of all this object's children.
    /// If no children are present, returns zero.
    uint32_t getLowestChildZOrder();

    /// Get highest Z-order of all this object's children.
    /// If no children are present, returns zero.
    uint32_t getHighestChildZOrder();

    template< typename ...args >
    void visitChildren(std::function<void(GUIObject&, args...)> functor)
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

    virtual std::unordered_set<EventID> registeredEvents() const override;

  protected:
    GUIObject* getParent();

    void setParent(GUIObject* parent);

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

    /// Handles a flag being set/cleared.
    /// This method is called by setFlag() if the value was changed.
    /// The default behavior is to do nothing.
    virtual void handleSetFlag_(std::string name, bool enabled);

    virtual bool onEvent_V(Event const& event) override final;

    bool onEventResized(EventResized const& event);
    bool onEventDragStarted(EventDragStarted const& event);
    bool onEventDragging(EventDragging const& event);
    bool onEventDragFinished(EventDragFinished const& event);

    virtual bool onEvent_V2(Event const& event) { return false; }

    /// Subscribe to parent events that all objects care about.
    void subscribeToParentEvents(Subject& parent);

    /// Subscribe to any additional events that we care about which are 
    /// emitted by a parent.
    /// The default behavior is to do nothing.
    virtual void subscribeToParentEvents_V(Subject& parent);

  private:
    /// The name of this object.
    std::string m_name;

    /// The parent of this object. Set to nullptr if the object has no parent.
    GUIObject* m_parent;

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
    std::unordered_map< std::string, std::unique_ptr<GUIObject> > m_children;

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
