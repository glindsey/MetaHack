#include "stdafx.h"

#include "views/EntityView.h"

#include "game/App.h"
#include "game/GameState.h"
#include "entity/EntityFactory.h"

EntityView::EntityView(EntityId entity)
  :
  Object({}),
  m_entityId(entity)
{
}

EntityView::~EntityView()
{}

void EntityView::setLocation(RealVec2 target_coords)
{
  if (target_coords != m_target_coords)
  {
    m_target_coords = target_coords;
  }
}

RealVec2 EntityView::getLocation()
{
  return m_target_coords;
}

void EntityView::setSize(RealVec2 target_size)
{
  if (target_size != m_target_size)
  {
    m_target_size = target_size;
  }
}

RealVec2 EntityView::getSize()
{
  return m_target_size;
}

EntityId EntityView::getEntity()
{
  return m_entityId;
}

EntityId const& EntityView::getEntity() const
{
  return m_entityId;
}