#include "stdafx.h"

#include "GUIShrinkHandle.h"

#include "App.h"
#include "IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  ShrinkHandle::ShrinkHandle(std::string name)
    :
    Object(name, IntVec2(0, 0))
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