#include "stdafx.h"

#include "GUIResizeHandle.h"

#include "App.h"
#include "IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  ResizeHandle::ResizeHandle(std::string name)
    :
    Object(name, { 0, 0 }, { s_handle_size - 2, s_handle_size - 2 })
  {}

  ResizeHandle::~ResizeHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ResizeHandle::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
    auto parent = get_parent();
    if (parent == nullptr)
    {
      return;
    }

    auto parent_size = parent->get_size();
    set_relative_location({ static_cast<int>(parent_size.x - s_handle_size),
                            static_cast<int>(parent_size.y - s_handle_size) });

    /// @todo FINISH ME
  }
  void ResizeHandle::handle_parent_size_changed_(Vec2u parent_size)
  {
  }
}; // end namespace metagui