#pragma once

#include "GUIObject.h"

namespace metagui
{
  class TitleBar : public GUIObject
  {
    friend class Window;

  public:
    virtual ~TitleBar();

  protected:
    TitleBar(Desktop& desktop, std::string name);

    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual bool onEvent_V(Event const& event) override;

  private:
  };
}; // end namespace metagui
