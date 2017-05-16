#include "stdafx.h"

#include "GUIObject.h"

#include <exception>

#include "AssertHelper.h"
#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"
#include "types/Color.h"
#include "utilities/CommonFunctions.h"
#include "utilities/MathUtils.h"

namespace metagui
{
  std::unordered_set<EventID> const GUIObject::s_events =
  {
    EventMoved::id,
    EventResized::id,
    EventDragStarted::id,
    EventDragging::id,
    EventDragFinished::id
  };

  GUIObject::GUIObject(std::string name,
                       std::unordered_set<EventID> const events) :
    Object(combine(s_events, events))
  {
  }

  GUIObject::GUIObject(std::string name,
                       std::unordered_set<EventID> const events,
                       IntVec2 location, UintVec2 size) :
    GUIObject(name, events)
  {
    setRelativeLocation(location);
    setSize(size);
  }

  GUIObject::GUIObject(std::string name,
                       std::unordered_set<EventID> const events,
                       sf::IntRect dimensions) :
    GUIObject(name, events)
  {
    setRelativeDimensions(dimensions);
  }

  GUIObject::~GUIObject()
  {
    // Remove child subject/observer relationships, if any.
    for (auto& childPair : m_children)
    {
      childPair.second->removeObserver(*this);
      removeObserver(*(childPair.second));
    }

    m_children.clear();

    // Remove parent subject/observer relationships, if any.
    if (m_parent != nullptr)
    {
      m_parent->removeObserver(*this);
      removeObserver(*m_parent);
    }

    // Unsubscribe from app events, if any.
    App::instance().removeObserver(*this);
  }

  std::string GUIObject::getName()
  {
    return m_name;
  }

  void GUIObject::setFocus(bool focus)
  {
    if ((m_cachedFlags.disabled == false) || (m_cachedFlags.hidden == false))
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
    if ((m_cachedFlags.disabled == false) || (m_cachedFlags.hidden == false))
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
    auto old_location = m_location;
    m_location = location;
    if (m_parent != nullptr)
    {
      m_parent->flagForRedraw();
    }

    broadcast(EventMoved(old_location, location));
  }

  UintVec2 GUIObject::getSize()
  {
    return m_size;
  }

  /// @todo Minimum texture size, configurable by subclass.
  void GUIObject::setSize(UintVec2 size)
  {
    // Do nothing if requested size is same as current size.
    /// @todo Re-enable this; disabled right now because we push a "resize"
    ///       event when new children are added.
    //if (m_size == size) return;

    auto oldSize = m_size;
    m_size = size;

    // Would be better not to create a texture if size is 0 in either dimension
    // but for the moment this is faster.
    /// @todo See if we can make this more elegant.
    if (size.x < 1) size.x = 1;
    if (size.y < 1) size.y = 1;

    if (m_parent != nullptr)
    {
      auto maxSize = ((m_cachedFlags.decor == true) ?
                      m_parent->getSize() :
                      m_parent->getChildAreaSize());

      if (size.x > maxSize.x) size.x = maxSize.x;
      if (size.y > maxSize.y) size.y = maxSize.y;
    }

    // Texture size should be the nearest power-of-2, for speed.
    UintVec2 newTextureSize{ nextPowerOfTwo(size.x), nextPowerOfTwo(size.y) };
    if (newTextureSize != m_bgTextureSize)
    {
      m_bgTextureSize = newTextureSize;
      m_bgTexture.reset(NEW sf::RenderTexture());
      m_bgTexture->create(m_bgTextureSize.x, m_bgTextureSize.y);
    }

    // Set "dirty" for both this object and its parent chain.
    flagForRedraw();
    broadcast(GUIObject::EventResized(oldSize, size));
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
    IntVec2 absoluteLocation = getRelativeLocation();

    if (m_parent != nullptr)
    {
      IntVec2 childAreaAbsoluteLocation =
        m_parent->getAbsoluteLocation();

      if (m_cachedFlags.decor != true)
      {
        childAreaAbsoluteLocation += m_parent->getChildAreaLocation();
      }

      absoluteLocation += childAreaAbsoluteLocation;
    }

    return absoluteLocation;
  }

  void GUIObject::setAbsoluteLocation(IntVec2 location)
  {
    IntVec2 relativeLocation = location;

    if (m_parent != nullptr)
    {
      IntVec2 childAreaAbsoluteLocation =
        m_parent->getAbsoluteLocation();

      if (m_cachedFlags.decor != true)
      {
        childAreaAbsoluteLocation += m_parent->getChildAreaLocation();
      }

      relativeLocation -= childAreaAbsoluteLocation;
    }

    setRelativeLocation(relativeLocation);
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
    m_zOrderMap.insert({ z_order, name });

    /// @todo Make this cleaner; this is a kludge to create a Resized event.
    setSize(m_size);

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
      std::unique_ptr<GUIObject> movedObject = std::move(m_children.at(name));
      // This erases the (now empty) unique_ptr from the map.
      m_children.erase(name);

      // This finds the child name in the Z-order map and gets rid of it.
      for (auto& iter = m_zOrderMap.begin(); iter != m_zOrderMap.end(); ++iter)
      {
        if (iter->second == name)
        {
          m_zOrderMap.erase(iter);
          break;
        }
      }

      // This clears the child's parent pointer and deregisters it from the
      // parent's events.
      movedObject->setParent(nullptr);

      CLOG(TRACE, "GUI") << "Removed child \"" << name <<
        "\" from parent \"" << getName() << "\"";

      // This returns the object we removed.
      return std::move(movedObject);
    }

    // Didn't find the object, so return an empty unique_ptr.
    return std::unique_ptr<GUIObject>();
  }

  uint32_t GUIObject::getLowestChildZOrder()
  {
    if (m_zOrderMap.size() > 0)
    {
      auto iter = m_zOrderMap.cbegin();
      return iter->first;
    }
    return 0;
  }

  uint32_t GUIObject::getHighestChildZOrder()
  {
    if (m_zOrderMap.size() > 0)
    {
      auto iter = m_zOrderMap.cend();
      --iter;
      return iter->first;
    }
    return 0;
  }

  void GUIObject::clearChildren()
  {
    m_children.clear();
    m_zOrderMap.clear();
  }

  IntVec2 GUIObject::getChildAreaLocation()
  {
    return{ 0, 0 };
  }

  UintVec2 GUIObject::getChildAreaSize()
  {
    return getSize();
  }

  bool GUIObject::render(sf::RenderTexture& parentTexture, int frame)
  {
    sf::RenderTexture& texture = *m_bgTexture;

    if (m_cachedFlags.hidden == false)
    {
      if ((m_needsRedraw == true) || (m_cachedFlags.animated == true))
      {
        texture.clear(Color::Transparent);
        draw(frame);

        if (m_cachedFlags.animated == false)
        {
          m_needsRedraw = false;
        }
      }

      // Create the RectangleShape that will be drawn onto the target.
      m_bgShape.setPosition(RealVec2(static_cast<float>(m_location.x), static_cast<float>(m_location.y)));
      m_bgShape.setSize(RealVec2(static_cast<float>(m_size.x), static_cast<float>(m_size.y)));
      m_bgShape.setTexture(&(m_bgTexture->getTexture()));
      m_bgShape.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));

      // Draw onto the parent.
      parentTexture.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(parentTexture.getSize().x), static_cast<float>(parentTexture.getSize().y))));
      parentTexture.draw(m_bgShape);

      parentTexture.display();
      return true;
    }
    else
    {
      return false;
    }
  }

  void GUIObject::draw(int frame)
  {
    sf::RenderTexture& texture = *m_bgTexture;

    /// Render self to our bg texture.
    drawPreChildren_(texture, frame);

    /// Render all child objects to our bg texture.
    for (auto& z_pair : m_zOrderMap)
    {
      m_children.at(z_pair.second)->render(texture, frame);
    }

    /// Render self after children are done.
    drawPostChildren_(texture, frame);

    texture.display();
  }

  void GUIObject::flagForRedraw()
  {
    m_needsRedraw = true;
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

  bool GUIObject::getFlag(std::string name, bool defaultValue)
  {
    if (m_flags.count(name) == 0)
    {
      setFlag(name, defaultValue);
    }
    return m_flags[name];
  }

  bool GUIObject::isHidden()
  {
    return m_cachedFlags.hidden;
  }

  bool GUIObject::isDisabled()
  {
    return m_cachedFlags.disabled;
  }

  void GUIObject::handleSetFlag(std::string name, bool value)
  {
    if (name == "hidden")
    {
      if (value == true)
      {
        setFocus(false);
      }
      m_cachedFlags.hidden = value;
    }
    else if (name == "disabled")
    {
      if (value == true)
      {
        setFocus(false);
      }
      m_cachedFlags.disabled = value;
    }
    else if (name == "animated")
    {
      m_cachedFlags.animated = value;
    }
    else if (name == "movable")
    {
      m_cachedFlags.movable = value;
    }
    else if (name == "decor")
    {
      m_cachedFlags.decor = value;
    }

    handleSetFlag_(name, value);
  }

  bool GUIObject::handlesPoint(IntVec2 point)
  {
    return containsPoint(point) && !childContainsPoint(point);
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

  bool GUIObject::childContainsPoint(IntVec2 point)
  {
    for (auto& childPair : m_children)
    {
      auto& child = childPair.second;
      if (!child->isHidden() && child->containsPoint(point))
      {
        return true;
      }
    }
    return false;
  }

  GUIObject * GUIObject::getParent()
  {
    return m_parent;
  }

  void GUIObject::setParent(GUIObject* parent)
  {
    if (m_parent != nullptr)
    {
      m_parent->removeObserver(*this);
      App::instance().removeObserver(*this);
    }

    m_parent = parent;

    if (parent != nullptr)
    {
      doEventSubscriptions(*parent);
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
    return m_beingDragged;
  }

  IntVec2 GUIObject::getDragStartLocation()
  {
    return m_dragStartLocation;
  }

  void GUIObject::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    // Default behavior is to do nothing.
  }

  void GUIObject::drawPostChildren_(sf::RenderTexture& texture, int frame)
  {
    // Default behavior is to do nothing.
  }

  void GUIObject::handleSetFlag_(std::string name, bool enabled)
  {
  }

  bool GUIObject::onEvent(Event const& event)
  {
    bool handled = false;

    // Do our own work.
    if (event.getId() == EventResized::id)
    {
      handled = onEventResized(static_cast<EventResized const&>(event));
    }
    else if (event.getId() == EventDragStarted::id)
    {
      handled = onEventDragStarted(static_cast<EventDragStarted const&>(event));
    }
    else if (event.getId() == EventDragging::id)
    {
      handled = onEventDragging(static_cast<EventDragging const&>(event));
    }
    else if (event.getId() == EventDragFinished::id)
    {
      handled = onEventDragFinished(static_cast<EventDragFinished const&>(event));
    }

    if (!handled)
    {
      handled = onEvent_V(event);
    }

    return handled;
  }

  bool GUIObject::onEventResized(EventResized const& event)
  {
    if (event.subject == m_parent)
    {
      // Default behavior is to run setSize again so we aren't larger than the
      // parent boundaries.
      setSize(m_size);
    }

    return false;
  }

  bool GUIObject::onEventDragStarted(EventDragStarted const& event)
  {
    if (handlesPoint(event.startLocation))
    {
      m_beingDragged = true;
      m_dragStartLocation = event.startLocation;
      m_dragStartAbsoluteLocation = getAbsoluteLocation();

      return false;
    }

    // Mark as handled so it is not passed to virtual handler.
    return true;
  }

  bool GUIObject::onEventDragging(EventDragging const& event)
  {
    if ((m_beingDragged == true) && (m_cachedFlags.movable == true))
    {
      auto moveAmount = event.currentLocation - m_dragStartLocation;
      auto newCoords = m_dragStartAbsoluteLocation + moveAmount;

      setAbsoluteLocation(newCoords);

      return false;
    }
    else
    {
      // Mark as handled so it is not passed to virtual handler.
      return true;
    }
  }

  bool GUIObject::onEventDragFinished(EventDragFinished const & event)
  {
    m_beingDragged = false;
    return false;
  }

  void GUIObject::doEventSubscriptions(Subject& parent)
  {
    // Subscribe to standard App events here.
    /// @todo Limit this to specific events and have subclasses add others
    ///       that they want.
    App::instance().addObserver(*this, EventID::All);

    // Subscribe to standard parent events here.
    parent.addObserver(*this, GUIObject::EventMoved::id);
    parent.addObserver(*this, GUIObject::EventResized::id);

    // Subscribe to any additional events.
    doEventSubscriptions_V(parent);
  }

  void GUIObject::doEventSubscriptions_V(Subject& parent)
  {
  }


}; // end namespace metagui