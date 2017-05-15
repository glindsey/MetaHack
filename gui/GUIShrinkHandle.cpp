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
  {}

  bool ShrinkHandle::onEvent_V2(Event const & event)
  {
    return false;
  }

}; // end namespace metagui