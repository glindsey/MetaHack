#include "stdafx.h"

#include "utilities/New.h"
#include "services/NullGraphicViews.h"

IGraphicViews::~IGraphicViews()
{}

IGraphicViews* IGraphicViews::getDefault()
{
  return NEW NullGraphicViews();
}
