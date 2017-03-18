#include "stdafx.h"

#include "GUICloseHandle.h"

#include "App.h"
#include "IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  CloseHandle::CloseHandle(std::string name)
    :
    Object(name, IntVec2(0, 0))
  {}

  void CloseHandle::handleParentSizeChanged_(UintVec2 parent_size)
  {
  }

  CloseHandle::~CloseHandle()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void CloseHandle::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
  }
}; // end namespace metagui