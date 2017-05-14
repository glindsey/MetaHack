#ifndef GUITITLEBAR_H
#define GUITITLEBAR_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class TitleBar :
    public GUIObject, public GUIObjectVisitable<TitleBar>
  {
    friend class Window;

  public:
    virtual ~TitleBar();

  protected:
    explicit TitleBar(std::string name);

    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual bool onEvent_PreChildren_NVI(Event const& event) override final;

  private:
  };
}; // end namespace metagui

#endif // GUITITLEBAR_H
