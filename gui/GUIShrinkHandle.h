#pragma once

#include "GUIObject.h"

namespace metagui
{
  class ShrinkHandle : public GUIObject
  {
    friend class Window;

  public:
    virtual ~ShrinkHandle();

  protected:
    ShrinkHandle(Desktop& desktop, std::string name);
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual bool onEvent_V(Event const& event) override;

  private:
  };
}; // end namespace metagui
