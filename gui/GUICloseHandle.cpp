#include "stdafx.h"

#include "GUICloseHandle.h"

#include "App.h"
#include "ConfigSettings.h"

namespace metagui
{
  CloseHandle::CloseHandle(StringKey name)
    :
    Object(name, sf::Vector2i(0, 0))
  {}

  void CloseHandle::handle_parent_size_changed_(sf::Vector2u parent_size)
  {
  }

  CloseHandle::~CloseHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void CloseHandle::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
  }
}; // end namespace metagui