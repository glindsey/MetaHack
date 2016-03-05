#include "stdafx.h"

#include "gui/GUIResizeHandle.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  ResizeHandle::ResizeHandle(std::string name)
    :
    Object(name, sf::Vector2i(0, 0))
  {}

  ResizeHandle::~ResizeHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ResizeHandle::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
  }
}; // end namespace metagui