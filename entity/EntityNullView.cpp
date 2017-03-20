#include "stdafx.h"

#include "entity/EntityNullView.h"

EntityNullView::EntityNullView(Entity& entity)
  :
  EntityView(entity)
{
}

EntityNullView::~EntityNullView()
{
}

void EntityNullView::draw(sf::RenderTarget& target,
                          bool use_lighting,
                          bool use_smoothing,
                          int frame)
{
}

std::string EntityNullView::getViewName()
{
  return "null";
}