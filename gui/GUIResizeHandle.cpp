#include "stdafx.h"

#include "GUIResizeHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"

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

    rect.setFillColor(getParent()->getFocus() ?
                      config.get("window_focused_border_color").as<Color>() :
                      config.get("window_border_color").as<Color>());
    rect.setOutlineThickness(0);
    rect.setPosition({ 0, 0 });
    rect.setSize({ static_cast<unsigned int>(s_handle_size), static_cast<unsigned int>(s_handle_size) });

    texture.draw(rect);

  }

  void ResizeHandle::handleParentSizeChanged_(UintVec2 parent_size)
  {
    setRelativeLocation({ static_cast<int>(parent_size.x - s_handle_size),
                          static_cast<int>(parent_size.y - s_handle_size) });
  }

  GUIEvent::Result ResizeHandle::handleGUIEventPostChildren_(GUIEventDragStarted & event)
  {
    auto parent = getParent();
    if (parent)
    {
      m_parent_size_start = parent->getSize();

      return GUIEvent::Result::Acknowledged;
    }

    return GUIEvent::Result::Ignored;
  }

  GUIEvent::Result ResizeHandle::handleGUIEventPostChildren_(GUIEventDragging & event)
  {
    GUIEvent::Result result = GUIEvent::Result::Ignored;

    auto parent = getParent();
    if (parent)
    {
      if (isBeingDragged() == true)
      {
        auto move_amount = event.current_location - getDragStartLocation();

        IntVec2 old_size{ static_cast<int>(m_parent_size_start.x), static_cast<int>(m_parent_size_start.y) };
        UintVec2 new_size;
        
        new_size.x = (move_amount.x > -(old_size.x)) ? (old_size.x + move_amount.x) : 0;
        new_size.y = (move_amount.y > -(old_size.y)) ? (old_size.y + move_amount.y) : 0;

        parent->setSize(new_size);
        result = GUIEvent::Result::Handled;
      }
    }

    return result;
  }

}; // end namespace metagui