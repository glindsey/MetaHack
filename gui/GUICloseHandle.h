#pragma once

#include "GUIObject.h"

namespace metagui
{
  class CloseHandle : public GUIObject
  {
    friend class Window;

  public:
    virtual ~CloseHandle();

  protected:
    CloseHandle(Desktop& desktop, std::string name);
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual bool onEvent_V(Event const& event) override final;

  private:
  };
}; // end namespace metagui
