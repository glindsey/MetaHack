#pragma once

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class Window :
    public GUIObject, public GUIObjectVisitable<Window>
  {
  public:
    Window(std::string name, IntVec2 location, UintVec2 size = UintVec2(0, 0));
    Window(std::string name, sf::IntRect dimensions);
    virtual ~Window();

    /// Get the upper-left corner of this object's child area, relative to
    /// its own upper-left corner, taking into account any decorator
    /// elements added to this window.
    virtual IntVec2 getChildAreaLocation() override final;

    /// Get the size of this window's child area, taking into account any
    /// decorator elements added to this window.
    virtual UintVec2 getChildAreaSize() override final;

  protected:
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;

    virtual void handleSetFlag_(std::string name, bool enabled) override;

    /// Temporary function used to render the contents of classes derived from
    /// GUIWindow until they can be properly replaced with child controls.
    virtual void drawContents_(sf::RenderTexture& texture, int frame);

    virtual bool onEvent_PostChildren_NVI(Event const& event) override;

  private:
    /// Border shape.
    sf::RectangleShape m_border_shape;

    /// Cached state of the "titlebar" flag.
    bool m_titlebar_cached = false;
  };
}; // end namespace metagui