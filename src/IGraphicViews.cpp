#include "stdafx.h"

#include "New.h"
#include "NullGraphicViews.h"

IGraphicViews::~IGraphicViews()
{}

IGraphicViews* IGraphicViews::getNull()
{
  return NEW NullGraphicViews();
}
