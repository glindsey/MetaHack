#include "stdafx.h"

#include "GUICollapseHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"
#include "types/Color.h"

namespace metagui
{
  CollapseHandle::CollapseHandle(std::string name) :
    GUIObject(name, {})
  {
  }

  CollapseHandle::~CollapseHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void CollapseHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = S<IConfigSettings>();

    if (getParent() == nullptr)
    {
      return;
    }

    auto parent_size = getParent()->getSize();

    /// @todo WRITE ME

    texture.display();
  }

  bool CollapseHandle::onEvent_V(Event const& event)
  {
    auto id = event.getId();

    if (id == EventResized::id)
    {
      if (event.subject == getParent())
      {
        auto& castEvent = static_cast<EventResized const&>(event);
        auto& config = S<IConfigSettings>();

        /// @todo WRITE ME

        //UintVec2 our_size
        //{
        //  castEvent.newSize.x,
        //  static_cast<unsigned int>(line_spacing_y + (text_offset.y * 2))
        //};

        //setSize(our_size);
      }
    }

    return false;
  }

}; // end namespace metagui