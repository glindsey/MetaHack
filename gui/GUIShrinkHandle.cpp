#include "stdafx.h"

#include "GUIShrinkHandle.h"

#include "App.h"
#include "ConfigSettings.h"

namespace metagui
{
  ShrinkHandle::ShrinkHandle(StringKey name)
    :
    Object(name, Vec2i(0, 0))
  {}

  ShrinkHandle::~ShrinkHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ShrinkHandle::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
  }
  void ShrinkHandle::handle_parent_size_changed_(Vec2u parent_size)
  {
  }
}; // end namespace metagui