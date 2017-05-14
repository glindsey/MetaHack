#ifndef GUICLOSEHANDLE_H
#define GUICLOSEHANDLE_H

#include "stdafx.h"

#include "GUIObject.h"

namespace metagui
{
  class CloseHandle :
    public GUIObject, public GUIObjectVisitable<CloseHandle>
  {
    friend class Window;

  public:
    virtual ~CloseHandle();

  protected:
    explicit CloseHandle(std::string name);
    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;
    virtual bool onEvent_PreChildren_NVI(Event const& event) override final;

  private:
  };
}; // end namespace metagui

#endif // GUICLOSEHANDLE_H
