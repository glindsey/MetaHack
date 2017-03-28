#include "stdafx.h"

#include "GUIShrinkHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  ShrinkHandle::ShrinkHandle(std::string name)
    :
    GUIObject(name, IntVec2(0, 0))
  {}

  ShrinkHandle::~ShrinkHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ShrinkHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
  }
  void ShrinkHandle::handleParentSizeChanged_(UintVec2 parent_size)
  {
  }
}; // end namespace metagui