#include "stdafx.h"

#include "GUICloseHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  CloseHandle::CloseHandle(std::string name)
    :
    GUIObject(name, IntVec2(0, 0))
  {}

  CloseHandle::~CloseHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void CloseHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
  }

  bool CloseHandle::onEvent_PreChildren_NVI(Event const& event)
  {
    return false;
  }

}; // end namespace metagui