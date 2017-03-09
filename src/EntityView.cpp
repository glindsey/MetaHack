#include "stdafx.h"

#include "EntityView.h"

#include "App.h"
#include "GameState.h"
#include "EntityPool.h"

EntityView::EntityView(Entity& entity)
  :
  m_entity(entity)
{
  //startObserving(entity);
}

EntityView::~EntityView()
{}

void EntityView::setLocation(Vec2f target_coords)
{
  if (target_coords != m_target_coords)
  {
    m_target_coords = target_coords;
  }
}

Vec2f EntityView::getLocation()
{
  return m_target_coords;
}

void EntityView::setSize(Vec2f target_size)
{
  if (target_size != m_target_size)
  {
    m_target_size = target_size;
  }
}

Vec2f EntityView::getSize()
{
  return m_target_size;
}

Entity& EntityView::getEntity()
{
  return m_entity;
}

Entity const& EntityView::getEntity() const
{
  return m_entity;
}