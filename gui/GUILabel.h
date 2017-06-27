#pragma once

#include "GUIObject.h"

namespace metagui
{
  class Label : public GUIObject
  {
  public:
    explicit Label(std::string name, IntVec2 location = IntVec2(0, 0));
    virtual ~Label();

  protected:
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;

  private:
  };
}; // end namespace metagui
