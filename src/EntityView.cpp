#include "stdafx.h"

#include "EntityView.h"

#include "App.h"
#include "GameState.h"
#include "EntityPool.h"

EntityView::EntityView(Entity& thing)
  :
  m_thing(thing)
{
  startObserving(thing);
}

EntityView::~EntityView()
{}

void EntityView::set_location(Vec2f target_coords)
{
  if (target_coords != m_target_coords)
  {
    m_target_coords = target_coords;
  }
}

Vec2f EntityView::get_location()
{
  return m_target_coords;
}

void EntityView::set_size(Vec2u target_size)
{
  if (target_size != m_target_size)
  {
    m_target_size = target_size;
  }
}

Vec2u EntityView::get_size()
{
  return m_target_size;
}

Entity& EntityView::get_thing()
{
  return m_thing;
}