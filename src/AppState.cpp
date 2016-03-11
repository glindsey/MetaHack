#include "stdafx.h"

#include "AppState.h"

#include "App.h"
#include "StateMachine.h"

AppState::AppState(StateMachine& state_machine,
                   metagui::RenderFunctor pre_render_functor,
                   metagui::RenderFunctor post_render_functor)
  :
  State{ state_machine }
{
  the_desktop.set_pre_child_render_functor(pre_render_functor);
  the_desktop.set_post_child_render_functor(post_render_functor);
}

AppState::~AppState()
{
  the_desktop.clear_children();
  the_desktop.clear_post_child_render_functor();
  the_desktop.clear_pre_child_render_functor();
}

bool AppState::render(sf::RenderTexture& texture, int frame)
{
  // Render the desktop.
  the_desktop.render(texture, frame);

  texture.display();
  return true;
}

SFMLEventResult AppState::handle_sfml_event(sf::Event & event)
{
  return SFMLEventResult::Ignored;
}