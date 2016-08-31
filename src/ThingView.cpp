#include "stdafx.h"

#include "ThingView.h"

#include "App.h"
#include "ConfigSettings.h"
#include "GameState.h"
#include "ThingManager.h"

ThingView::ThingView()
  :
  m_thing_id(ThingManager::get_mu())
{}

ThingView::ThingView(ThingId thing_id)
  :
  m_thing_id(thing_id)
{}

ThingView::~ThingView()
{}

void ThingView::set_thing_id(ThingId thing_id)
{
  if (thing_id != m_thing_id)
  {
    m_thing_id = thing_id;
    reset_cached_render_data();
  }
}

ThingId ThingView::get_thing_id()
{
  return m_thing_id;
}

void ThingView::set_location(Vec2f target_coords)
{
  if (target_coords != m_target_coords)
  {
    m_target_coords = target_coords;
    reset_cached_render_data();
  }
}

Vec2f ThingView::get_location()
{
  return m_target_coords;
}

void ThingView::set_size(Vec2u target_size)
{
  if (target_size != m_target_size)
  {
    m_target_size = target_size;
    reset_cached_render_data();
  }
}

Vec2u ThingView::get_size()
{
  return m_target_size;
}

Thing& ThingView::get_thing()
{
  return GAME.get_things().get(m_thing_id);
}