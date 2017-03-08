#include "stdafx.h"

#include "EntityNullView.h"

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

std::string EntityNullView::get_view_name()
{
  return "null";
}