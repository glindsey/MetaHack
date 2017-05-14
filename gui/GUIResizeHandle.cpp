#include "stdafx.h"

#include "GUIResizeHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"
#include "types/Color.h"

namespace metagui
{
  ResizeHandle::ResizeHandle(std::string name)
    :
    GUIObject(name, { 0, 0 }, { s_handle_size - 2, s_handle_size - 2 })
  {}

  ResizeHandle::~ResizeHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ResizeHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = Service<IConfigSettings>::get();

    auto parent = getParent();
    if (parent == nullptr)
    {
      return;
    }

    /// @todo FINISH ME. Right now this is just a dead-stupid square.
    sf::RectangleShape rect;

    auto focused_color = config.get("window-focused-border-color").get<Color>();
    auto unfocused_color = config.get("window-border-color").get<Color>();

    rect.setFillColor(getParent()->getFocus() ? focused_color : unfocused_color);
    rect.setOutlineThickness(0);
    rect.setPosition({ 0, 0 });
    rect.setSize({ static_cast<unsigned int>(s_handle_size), static_cast<unsigned int>(s_handle_size) });

    texture.draw(rect);

  }

  bool ResizeHandle::onEvent_PreChildren_NVI(Event const& event)
  {
    auto id = event.getId();
    if (id == EventResized::id())
    {
      if (event.subject == getParent())
      {
        auto& castEvent = static_cast<EventResized const&>(event);
        setRelativeLocation({ static_cast<int>(castEvent.new_size.x - s_handle_size),
                              static_cast<int>(castEvent.new_size.y - s_handle_size) });
      }
    }

    return false;
  }

  bool ResizeHandle::onEvent_PostChildren_NVI(Event const& event)
  {
    auto id = event.getId();
    auto parent = getParent();
    if (id == EventDragStarted::id() && parent)
    {
      m_parent_size_start = parent->getSize();
    }
    else if (id == EventDragging::id() && parent && isBeingDragged())
    {
      auto& castEvent = static_cast<EventDragging const&>(event);
      auto move_amount = castEvent.current_location - getDragStartLocation();

      IntVec2 old_size{ static_cast<int>(m_parent_size_start.x), static_cast<int>(m_parent_size_start.y) };
      UintVec2 new_size;

      new_size.x = (move_amount.x > -(old_size.x)) ? (old_size.x + move_amount.x) : 0;
      new_size.y = (move_amount.y > -(old_size.y)) ? (old_size.y + move_amount.y) : 0;

      parent->setSize(new_size);
    }

    return false;
  }

}; // end namespace metagui