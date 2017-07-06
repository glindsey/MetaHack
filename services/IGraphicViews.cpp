#include "stdafx.h"

#include "utilities/New.h"
#include "views/NullGraphicViews.h"

IGraphicViews::~IGraphicViews()
{}

IGraphicViews* IGraphicViews::getDefault()
{
  return NEW NullGraphicViews();
}
