#include "stdafx.h"

#include "GUIResizeHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"
#include "types/Color.h"

namespace metagui
{
  ResizeHandle::ResizeHandle(std::string name)
    :
    GUIObject(name, {}, { 0, 0 }, { s_handleSize - 2, s_handleSize - 2 })
  {}

  ResizeHandle::~ResizeHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ResizeHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = S<IConfigSettings>();

    auto parent = getParent();
    if (parent == nullptr)
    {
      return;
    }

    /// @todo FINISH ME. Right now this is just a dead-stupid square.
    sf::RectangleShape rect;

    auto focusedColor = config.get("window-focused-border-color").get<Color>();
    auto unfocusedColor = config.get("window-border-color").get<Color>();

    rect.setFillColor(getParent()->getFocus() ? focusedColor : unfocusedColor);
    rect.setOutlineThickness(0);
    rect.setPosition({ 0, 0 });
    rect.setSize({ static_cast<unsigned int>(s_handleSize), static_cast<unsigned int>(s_handleSize) });

    texture.draw(rect);

  }

  bool ResizeHandle::onEvent_V(Event const& event)
  {
    auto id = event.getId();
    auto parent = getParent();
    if (id == EventResized::id)
    {
      if (event.subject == getParent())
      {
        auto& castEvent = static_cast<EventResized const&>(event);
        setRelativeLocation({ static_cast<int>(castEvent.newSize.x - s_handleSize),
                              static_cast<int>(castEvent.newSize.y - s_handleSize) });
      }
    }
    else if (id == EventDragStarted::id && parent)
    {
      m_parentSizeStart = parent->getSize();
    }
    else if (id == EventDragging::id && parent && isBeingDragged())
    {
      auto& castEvent = static_cast<EventDragging const&>(event);
      auto moveAmount = castEvent.currentLocation - getDragStartLocation();

      IntVec2 oldSize{ static_cast<int>(m_parentSizeStart.x), static_cast<int>(m_parentSizeStart.y) };
      UintVec2 newSize;

      newSize.x = (moveAmount.x > -(oldSize.x)) ? (oldSize.x + moveAmount.x) : 0;
      newSize.y = (moveAmount.y > -(oldSize.y)) ? (oldSize.y + moveAmount.y) : 0;

      parent->setSize(newSize);
    }

    return false;
  }

}; // end namespace metagui