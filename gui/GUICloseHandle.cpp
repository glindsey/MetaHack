#include "stdafx.h"

#include "GUICloseHandle.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"

namespace metagui
{
  CloseHandle::CloseHandle(std::string name) :
    GUIObject(name, {})
  {}

  CloseHandle::~CloseHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void CloseHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    /// @todo WRITE ME
  }

  bool CloseHandle::onEvent_V(Event const& event)
  {
    /// @todo WRITE ME
    return false;
  }

}; // end namespace metagui