#include "stdafx.h"

#include "ThingView.h"

#include "App.h"
#include "GameState.h"
#include "ThingManager.h"

ThingView::ThingView(Thing& thing)
  :
  m_thing(thing)
{
  startObserving(thing);
}

ThingView::~ThingView()
{}

void ThingView::set_location(Vec2f target_coords)
{
  if (target_coords != m_target_coords)
  {
    m_target_coords = target_coords;
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
  }
}

Vec2u ThingView::get_size()
{
  return m_target_size;
}

Thing& ThingView::get_thing()
{
  return m_thing;
}