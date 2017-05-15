#include "stdafx.h"

#include "entity/EntityNullView.h"

EntityNullView::EntityNullView(Entity& entity)
  :
  EntityView(entity)
{
}

bool EntityNullView::onEvent_V(Event const& event)
{
  return false;
}

EntityNullView::~EntityNullView()
{
}

void EntityNullView::draw(sf::RenderTarget& target,
                          SystemLighting* lighting,
                          bool use_smoothing,
                          int frame)
{
}

std::string EntityNullView::getViewName()
{
  return "null";
}