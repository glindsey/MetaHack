#ifndef APPSTATE_H
#define APPSTATE_H

#include "stdafx.h"

#include "Renderable.h"
#include "SFMLEventHandler.h"
#include "State.h"

#include "GUIDesktop.h"

// Forward declarations
class StateMachine;

class AppState :
  public State
{
public:
  AppState(StateMachine& state_machine,
           metagui::RenderFunctor preDesktopRenderFunctor = metagui::RenderFunctor(),
           metagui::RenderFunctor postDesktopRenderFunctor = metagui::RenderFunctor());

  virtual ~AppState();

  virtual bool render(sf::RenderTexture& texture, int frame) override final;
  virtual SFMLEventResult handle_sfml_event(sf::Event& event) override;

protected:
private:
  metagui::RenderFunctor m_preDesktopRenderFunctor;
  metagui::RenderFunctor m_postDesktopRenderFunctor;
};

#endif // APPSTATE_H
