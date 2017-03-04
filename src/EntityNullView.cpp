#include "stdafx.h"

#include "EntityNullView.h"

EntityNullView::EntityNullView(Entity& thing)
  :
  EntityView(thing)
{
}

EntityNullView::~EntityNullView()
{
}

void EntityNullView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
