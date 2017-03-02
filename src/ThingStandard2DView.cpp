#include "stdafx.h"

#include "IConfigSettings.h"
#include "ThingStandard2DView.h"
#include "RNGUtils.h"
#include "Service.h"

#include "ShaderEffect.h"

ThingStandard2DView::ThingStandard2DView(Thing& thing)
  :
  ThingView(thing)
{
}

ThingStandard2DView::~ThingStandard2DView()
{
}

void ThingStandard2DView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
