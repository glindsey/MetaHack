#include "stdafx.h"

#include "GUIObject.h"

#include <exception>

#include "AssertHelper.h"
#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  GUIObject::GUIObject(std::string name, IntVec2 location, UintVec2 size)
  {
    m_name = name;
    m_parent = nullptr;
    setRelativeLocation(location);
    setSize(size);
  }

  GUIObject::GUIObject(std::string name, sf::IntRect dimensions)
  {
    m_name = name;
    m_parent = nullptr;
    setRelativeDimensions(dimensions);
  }

  GUIObject::~GUIObject()
  {
    //dtor
  }

  std::string GUIObject::getName()
  {
    return m_name;
  }

  void GUIObject::setFocus(bool focus)
  {
    if ((m_cached_flags.disabled == false) || (m_cached_flags.hidden == false))
    {
      if (m_parent != nullptr)
      {
        m_parent->clearChildFocuses();
      }

      setFocusOnly(focus);
    }
  }

  bool GUIObject::getFocus()
  {
    return m_focus;
  }

  bool GUIObject::getGlobalFocus()
  {
    bool global_focus = m_focus;

    if ((global_focus == true) && (m_parent != nullptr))
    {
      global_focus &= m_parent->getGlobalFocus();
    }

    return global_focus;
  }

  void GUIObject::setGlobalFocus(bool focus)
  {
    if ((m_cached_flags.disabled == false) || (m_cached_flags.hidden == false))
    {
      if (m_parent != nullptr)
      {
        m_parent->setGlobalFocus(focus);
      }

      setFocus(focus);
    }
  }

  void GUIObject::setText(std::string text)
  {
    if (m_text != text) flagForRedraw();
    m_text = text;
  }

  std::string GUIObject::getText()
  {
    return m_text;
  }

  IntVec2 GUIObject::getRelativeLocation()
  {
    return m_location;
  }

  void GUIObject::setRelativeLocation(IntVec2 location)
  {
    m_location = location;
    if (m_parent != nullptr)
    {
      m_parent->flagForRedraw();
    }
  }

  UintVec2 GUIObject::getSize()
  {
    return m_size;
  }

  /// @todo Minimum texture size, configurable by subclass.
  void GUIObject::setSize(UintVec2 size)
  {
    // Do nothing if requested size is same as current size.
    if (m_size == size) return;

    m_size = size;

    // Would be better not to create a texture if size is 0 in either dimension
    // but for the moment this is faster.
    /// @todo See if we can make this more elegant.
    if (size.x < 1) size.x = 1;
    if (size.y < 1) size.y = 1;

    if (m_parent != nullptr)
    {
      auto max_size = ((m_cached_flags.decor == true) ?
                       m_parent->getSize() :
                       m_parent->getChildAreaSize());

      if (size.x > max_size.x) size.x = max_size.x;
      if (size.y > max_size.y) size.y = max_size.y;
    }

    // Texture size should be the nearest power-of-2, for speed.
    UintVec2 new_texture_size{ next_power_of_two(size.x), next_power_of_two(size.y) };
    if (new_texture_size != m_bg_texture_size)
    {
      m_bg_texture_size = new_texture_size;
      m_bg_texture.reset(NEW sf::RenderTexture());
      m_bg_texture->create(m_bg_texture_size.x, m_bg_texture_size.y);
    }
    
    // Set "dirty" for both this object and its parent.
    // (Right now flagForRedraw() will automatically set the flag on the 
    //  parent chain, but I'll leave the other line commented out just in
    //  case I change the behavior later.)
    flagForRedraw();
    //if (m_parent != nullptr) m_parent->flagForRedraw();

    // Inform children of the parent size change.
    for (auto& child : m_children)
    {
      child.second->handleParentSizeChanged_(size);
    }
  }

  sf::IntRect GUIObject::getRelativeDimensions()
  {
    sf::IntRect dimensions;
    IntVec2 location = getRelativeLocation();
    UintVec2 size = getSize();
    dimensions.left = location.x;
    dimensions.top = location.y;
    dimensions.width = size.x;
    dimensions.height = size.y;
    return dimensions;
  }

  void GUIObject::setRelativeDimensions(sf::IntRect dimensions)
  {
    setRelativeLocation({ dimensions.left, dimensions.top });
    setSize({ static_cast<unsigned int>(dimensions.width),
               static_cast<unsigned int>(dimensions.height) });
  }

  IntVec2 GUIObject::getAbsoluteLocation()
  {
    IntVec2 absolute_location = getRelativeLocation();

    if (m_parent != nullptr)
    {
      IntVec2 child_area_absolute_location =
        m_parent->getAbsoluteLocation();

      if (m_cached_flags.decor != true)
      {
        child_area_absolute_location += m_parent->getChildAreaLocation();
      }

      absolute_location += child_area_absolute_location;
    }

    return absolute_location;
  }

  void GUIObject::setAbsoluteLocation(IntVec2 location)
  {
    IntVec2 relative_location = location;

    if (m_parent != nullptr)
    {
      IntVec2 child_area_absolute_location =
        m_parent->getAbsoluteLocation();

      if (m_cached_flags.decor != true)
      {
        child_area_absolute_location += m_parent->getChildAreaLocation();
      }

      relative_location -= child_area_absolute_location;
    }

    setRelativeLocation(relative_location);
  }

  sf::IntRect GUIObject::getAbsoluteDimensions()
  {
    sf::IntRect dimensions;
    IntVec2 location = getAbsoluteLocation();
    UintVec2 size = getSize();
    dimensions.left = location.x;
    dimensions.top = location.y;
    dimensions.width = size.x;
    dimensions.height = size.y;
    return dimensions;
  }

  GUIObject* GUIObject::addChild(std::unique_ptr<GUIObject> child, uint32_t z_order)
  {
    Assert("GUI", child, "tried to add null child to a GUI object");

    std::string name = child->getName();

    Assert("GUI", !childExists(name), "Tried to add already-present child \"" + name + "\" of GUI object \"" + getName() + "\"");
    
    // This sets the child's parent pointer and registers it with the
    // parent's events.
    child->setParent(this);
    auto child_ptr = child.get();

    // This odd syntax is in order to work around VS compiler bug when having
    // a unique_ptr as a map value. See:
    // https://stackoverflow.com/questions/21056872/c-stdunique-ptr-wont-compile-in-map
    m_children.insert<ChildMap::value_type>(ChildMap::value_type(name, std::move(child)));
    m_zorder_map.insert({ z_order, name });

    child_ptr->handleParentSizeChanged_(getSize());

    CLOG(TRACE, "GUI") << "Added child \"" << name <<
      "\" (with Z-order " << z_order <<
      ") to parent \"" << getName() << "\"";

    return child_ptr;
  }

  GUIObject* GUIObject::addChild(std::unique_ptr<GUIObject> child)
  {
    uint32_t z_order = getHighestChildZOrder() + 1;
    return addChild(std::move(child), z_order);
  }

  GUIObject* GUIObject::addChildTop(std::unique_ptr<GUIObject> child)
  {
    uint32_t z_order = getLowestChildZOrder() - 1;
    return addChild(std::move(child), z_order);
  }

  bool GUIObject::childExists(std::string name)
  {
    return (m_children.count(name) > 0);
  }

  GUIObject& GUIObject::getChild(std::string name)
  {
    if (childExists(name))
    {
      return *m_children.at(name);
    }

    throw std::runtime_error("Tried to get non-existent child \"" + name + "\" of GUI object \"" + getName() + "\"");
  }

  std::unique_ptr<GUIObject> GUIObject::removeChild(std::string name)
  {
    if (childExists(name))
    {
      // This moves the object out of the stored unique_ptr.
      std::unique_ptr<GUIObject> moved_object = std::move(m_children.at(name));
      // This erases the (now empty) unique_ptr from the map.
      m_children.erase(name);

      // This finds the child name in the Z-order map and gets rid of it.
      for (auto& iter = m_zorder_map.begin(); iter != m_zorder_map.end(); ++iter)
      {
        if (iter->second == name)
        {
          m_zorder_map.erase(iter);
          break;
        }
      }

      // This clears the child's parent pointer and deregisters it from the
      // parent's events.
      moved_object->setParent(nullptr);

      CLOG(TRACE, "GUI") << "Removed child \"" << name <<
        "\" from parent \"" << getName() << "\"";

      // This returns the object we removed.
      return std::move(moved_object);
    }

    // Didn't find the object, so return an empty unique_ptr.
    return std::unique_ptr<GUIObject>();
  }

  uint32_t GUIObject::getLowestChildZOrder()
  {
    if (m_zorder_map.size() > 0)
    {
      auto iter = m_zorder_map.cbegin();
      return iter->first;
    }
    return 0;
  }

  uint32_t GUIObject::getHighestChildZOrder()
  {
    if (m_zorder_map.size() > 0)
    {
      auto iter = m_zorder_map.cend();
      --iter;
      return iter->first;
    }
    return 0;
  }

  void GUIObject::clearChildren()
  {
    m_children.clear();
    m_zorder_map.clear();
  }

  IntVec2 GUIObject::getChildAreaLocation()
  {
    return{ 0, 0 };
  }

  UintVec2 GUIObject::getChildAreaSize()
  {
    return getSize();
  }

  bool GUIObject::render(sf::RenderTexture& parent_texture, int frame)
  {
    sf::RenderTexture& our_texture = *m_bg_texture;

    if (m_cached_flags.hidden == false)
    {
      if ((m_flag_for_redraw == true) || (m_cached_flags.animated == true))
      {
        our_texture.clear(sf::Color::Transparent);
        draw(frame);

        if (m_cached_flags.animated == false)
        {
          m_flag_for_redraw = false;
        }
      }

      // Create the RectangleShape that will be drawn onto the target.
      m_bg_shape.setPosition(RealVec2(static_cast<float>(m_location.x), static_cast<float>(m_location.y)));
      m_bg_shape.setSize(RealVec2(static_cast<float>(m_size.x), static_cast<float>(m_size.y)));
      m_bg_shape.setTexture(&(m_bg_texture->getTexture()));
      m_bg_shape.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));

      // Draw onto the parent.
      parent_texture.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(parent_texture.getSize().x), static_cast<float>(parent_texture.getSize().y))));
      parent_texture.draw(m_bg_shape);

      parent_texture.display();
      return true;
    }
    else
    {
      return false;
    }
  }

  void GUIObject::draw(int frame)
  {
    sf::RenderTexture& our_texture = *m_bg_texture;

    /// Render self to our bg texture.
    drawPreChildren_(our_texture, frame);

    /// Render all child objects to our bg texture.
    for (auto& z_pair : m_zorder_map)
    {
      m_children.at(z_pair.second)->render(our_texture, frame);
    }

    /// Render self after children are done.
    drawPostChildren_(our_texture, frame);

    our_texture.display();
  }

  void GUIObject::flagForRedraw()
  {
    m_flag_for_redraw = true;
    if (m_parent)
    {
      m_parent->flagForRedraw();
    }
  }

  void GUIObject::setFlag(std::string name, bool value)
  {
    if ((m_flags.count(name) == 0) || (m_flags[name] != value))
    {
      m_flags[name] = value;
      handleSetFlag(name, value);
    }
  }

  bool GUIObject::getFlag(std::string name, bool default_value)
  {
    if (m_flags.count(name) == 0)
    {
      setFlag(name, default_value);
    }
    return m_flags[name];
  }

  void GUIObject::handleSetFlag(std::string name, bool value)
  {
    if (name == "hidden")
    {
      if (value == true)
      {
        setFocus(false);
      }
      m_cached_flags.hidden = value;
    }
    else if (name == "disabled")
    {
      if (value == true)
      {
        setFocus(false);
      }
      m_cached_flags.disabled = value;
    }
    else if (name == "animated")
    {
      m_cached_flags.animated = value;
    }
    else if (name == "movable")
    {
      m_cached_flags.movable = value;
    }
    else if (name == "decor")
    {
      m_cached_flags.decor = value;
    }

    handleSetFlag_(name, value);
  }

  bool GUIObject::containsPoint(IntVec2 point)
  {
    auto left = getAbsoluteLocation().x;
    auto top = getAbsoluteLocation().y;
    auto right = left + (static_cast<int>(getSize().x) - 1);
    auto bottom = top + (static_cast<int>(getSize().y) - 1);

    return ((point.x >= left) && (point.x <= right) &&
      (point.y >= top) && (point.y <= bottom));
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren(GUIEventDragFinished& event)
  {
    GUIEvent::Result result = handleGUIEventPreChildren_(event);

    return result;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren(GUIEventDragFinished& event)
  {
    m_being_dragged = false;

    GUIEvent::Result result = handleGUIEventPostChildren_(event);

    return result;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren(GUIEventDragStarted& event)
  {
    GUIEvent::Result result;

    if (containsPoint(event.start_location))
    {
      result = handleGUIEventPreChildren_(event);
    }
    else
    {
      // We "Ignore" the event so it is not passed to children.
      result = GUIEvent::Result::Ignored;
    }

    return result;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren(GUIEventDragStarted& event)
  {
    GUIEvent::Result result;

    if (containsPoint(event.start_location))
    {
      m_being_dragged = true;
      m_drag_start_location = event.start_location;
      m_absolute_location_drag_start = getAbsoluteLocation();

      result = handleGUIEventPostChildren_(event);
    }
    else
    {
      // We "Ignore" the event so it is not passed to children.
      result = GUIEvent::Result::Ignored;
    }

    return result;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren(GUIEventDragging& event)
  {
    // We "Acknowledge" the event so it is passed to children.
    return handleGUIEventPreChildren_(event);
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren(GUIEventDragging& event)
  {
    GUIEvent::Result result = handleGUIEventPostChildren_(event);

    if (result != GUIEvent::Result::Handled)
    {
      // If we got here, all children ignored the event (or there are no
      // children), and there's no subclass override -- so we want to 
      // process it if we are movable.
      if ((m_being_dragged == true) && (m_cached_flags.movable == true))
      {
        auto move_amount = event.current_location - m_drag_start_location;
        auto new_coords = m_absolute_location_drag_start + move_amount;

        setAbsoluteLocation(new_coords);
        result = GUIEvent::Result::Handled;
      }
      else
      {
        result = GUIEvent::Result::Ignored;
      }
    }

    return result;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren(GUIEventKeyPressed& event)
  {
    return handleGUIEventPreChildren_(event);
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren(GUIEventKeyPressed& event)
  {
    return handleGUIEventPostChildren_(event);
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren(GUIEventMouseDown& event)
  {
    return handleGUIEventPreChildren_(event);
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren(GUIEventMouseDown& event)
  {
    return handleGUIEventPostChildren_(event);
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren(GUIEventResized& event)
  {
    return handleGUIEventPreChildren_(event);
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren(GUIEventResized& event)
  {
    return handleGUIEventPostChildren_(event);
  }

  std::unordered_set<EventID> GUIObject::registeredEvents() const
  {
    auto events = Subject::registeredEvents();
    /// @todo Add our own events here
    return events;
  }

  GUIObject * GUIObject::getParent()
  {
    return m_parent;
  }

  void GUIObject::setParent(GUIObject* parent)
  {
    if (m_parent != nullptr)
    {
      m_parent->removeObserver(*this, EventID::All);
    }

    m_parent = parent;

    if (parent != nullptr)
    {
      /// @todo Handle setting event priorities
      parent->subscribeToParentEvents(*this, 0 /*parent->getChildZOrder(this)*/);
    }

    if (m_parent) m_parent->flagForRedraw();
  }

  void GUIObject::clearChildFocuses()
  {
    for (auto& child_pair : m_children)
    {
      child_pair.second->setFocusOnly(false);
    }
  }

  void GUIObject::setFocusOnly(bool focus)
  {
    if (m_focus != focus) flagForRedraw();
    m_focus = focus;
  }

  bool GUIObject::isBeingDragged()
  {
    return m_being_dragged;
  }

  IntVec2 GUIObject::getDragStartLocation()
  {
    return m_drag_start_location;
  }

  void GUIObject::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    // Default behavior is to do nothing.
  }

  void GUIObject::drawPostChildren_(sf::RenderTexture& texture, int frame)
  {
    // Default behavior is to do nothing.
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren_(GUIEventDragFinished& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren_(GUIEventDragFinished& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren_(GUIEventDragStarted& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren_(GUIEventDragStarted& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren_(GUIEventDragging& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren_(GUIEventDragging& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren_(GUIEventKeyPressed& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren_(GUIEventKeyPressed& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren_(GUIEventMouseDown& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren_(GUIEventMouseDown& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPreChildren_(GUIEventResized& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  GUIEvent::Result GUIObject::handleGUIEventPostChildren_(GUIEventResized& event)
  {
    return GUIEvent::Result::Acknowledged;
  }

  void GUIObject::handleSetFlag_(std::string name, bool enabled)
  {}

  void GUIObject::handleParentSizeChanged_(UintVec2 parent_size)
  {
  }

  EventResult GUIObject::onEvent_NVI(Event const& event)
  {
    EventResult result{ EventHandled::Yes, ContinueBroadcasting::Yes };

    // Pre-child handling.
    result.continue_broadcasting = (onEvent_NVI_PreChildren(event) ? ContinueBroadcasting::Yes : ContinueBroadcasting::No);
    if (result.continue_broadcasting == ContinueBroadcasting::Yes)
    {
      // Forward to children.
      result.continue_broadcasting = (forward(event) ? ContinueBroadcasting::Yes : ContinueBroadcasting::No);

      if (result.continue_broadcasting == ContinueBroadcasting::Yes)
      {
        // Post-child handling.
        result.continue_broadcasting = (onEvent_NVI_PostChildren(event) ? ContinueBroadcasting::Yes : ContinueBroadcasting::No);
      }
    }

    return result;
  }

  bool GUIObject::onEvent_NVI_PreChildren(Event const & event)
  {
    return true;
  }

  bool GUIObject::onEvent_NVI_PostChildren(Event const & event)
  {
    return true;
  }

  void GUIObject::subscribeToParentEvents(Subject const& parent, int priority)
  {
    /// @todo Subscribe to standard parent events here.

    // Subscribe to any additional events.
    subscribeToParentEvents_(parent, priority);
  }

  void GUIObject::subscribeToParentEvents_(Subject const& parent, int priority)
  {}


}; // end namespace metagui