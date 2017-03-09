#include "stdafx.h"

#include "AppState.h"

#include "App.h"
#include "StateMachine.h"

AppState::AppState(StateMachine& state_machine,
                   metagui::RenderFunctor preDesktopRenderFunctor,
                   metagui::RenderFunctor postDesktopRenderFunctor)
  :
  State{ state_machine },
  m_preDesktopRenderFunctor{ preDesktopRenderFunctor },
  m_postDesktopRenderFunctor{ postDesktopRenderFunctor }
{}

AppState::~AppState()
{
  the_desktop.clearChildren();
}

bool AppState::render(sf::RenderTexture& texture, int frame)
{
  texture.clear();

  if (m_preDesktopRenderFunctor) m_preDesktopRenderFunctor(texture, frame);
  the_desktop.render(texture, frame);
  if (m_postDesktopRenderFunctor) m_postDesktopRenderFunctor(texture, frame);

  texture.display();
  return true;
}

SFMLEventResult AppState::handle_sfml_event(sf::Event & event)
{
  return SFMLEventResult::Ignored;
}