#include "stdafx.h"

#include "services/DefaultPaths.h"

IPaths::~IPaths()
{}

IPaths* IPaths::getDefault()
{
  return NEW DefaultPaths();
}
