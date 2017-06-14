#include "stdafx.h"

#include "entity/EntityNullView.h"

EntityNullView::EntityNullView(EntityId entity)
  :
  EntityView(entity)
{
}

bool EntityNullView::onEvent(Event const& event)
{
  return false;
}

EntityNullView::~EntityNullView()
{
}

void EntityNullView::draw(sf::RenderTarget& target,
                          Systems::Lighting* lighting,
                          bool use_smoothing,
                          int frame)
{
}

std::string EntityNullView::getViewName()
{
  return "null";
}