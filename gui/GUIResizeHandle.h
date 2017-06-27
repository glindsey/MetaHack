#pragma once

#include "GUIObject.h"

namespace metagui
{
  class ResizeHandle : public GUIObject
  {
    friend class Window;

  public:
    virtual ~ResizeHandle();

  protected:
    explicit ResizeHandle(std::string name);
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;

    virtual bool onEvent_V(Event const& event) override;

  private:
    /// Size of the resize handle.
    /// @todo This should be moved into a central config file.
    static unsigned int const s_handleSize = 10;

    /// The starting parent size before the resize drag began.
    UintVec2 m_parentSizeStart;

  };
}; // end namespace metagui
