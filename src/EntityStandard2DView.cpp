#include "stdafx.h"

#include "IConfigSettings.h"
#include "EntityStandard2DView.h"
#include "RNGUtils.h"
#include "Service.h"

#include "ShaderEffect.h"

EntityStandard2DView::EntityStandard2DView(Entity& thing)
  :
  EntityView(thing)
{
}

EntityStandard2DView::~EntityStandard2DView()
{
}

void EntityStandard2DView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
