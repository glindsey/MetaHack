#include "stdafx.h"

#include "GUIShrinkHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"

namespace metagui
{
  ShrinkHandle::ShrinkHandle(Desktop& desktop, std::string name) :
    GUIObject(desktop, name, {})
  {}

  ShrinkHandle::~ShrinkHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void ShrinkHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    /// @todo WRITE ME
  }

  bool ShrinkHandle::onEvent_V(Event const & event)
  {
    /// @todo WRITE ME
    return false;
  }

}; // end namespace metagui