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
           metagui::RenderFunctor pre_render_functor = metagui::RenderFunctor(),
           metagui::RenderFunctor post_render_functor = metagui::RenderFunctor());

  virtual ~AppState();

  virtual bool render(sf::RenderTexture& texture, int frame) override final;
  virtual SFMLEventResult handle_sfml_event(sf::Event& event) override;

protected:
private:
};

#endif // APPSTATE_H
