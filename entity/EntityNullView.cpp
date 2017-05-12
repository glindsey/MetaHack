#include "stdafx.h"

#include "entity/EntityNullView.h"

EntityNullView::EntityNullView(Entity& entity)
  :
  EntityView(entity)
{
}

EventResult EntityNullView::onEvent_NVI(Event const & event)
{
  return{ EventHandled::Yes, ContinueBroadcasting::Yes };
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