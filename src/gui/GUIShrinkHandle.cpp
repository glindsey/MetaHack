#include "stdafx.h"

#include "gui/GUIShrinkHandle.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  ShrinkHandle::ShrinkHandle(std::string name)
    :
    Object(name, sf::Vector2i(0, 0))
  {}

  ShrinkHandle::~ShrinkHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ShrinkHandle::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
  }
  void ShrinkHandle::handle_parent_size_changed_(sf::Vector2u parent_size)
  {
  }
}; // end namespace metagui