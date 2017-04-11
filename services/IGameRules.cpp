#include "stdafx.h"

#include "services/NullGameRules.h"

IGameRules::~IGameRules()
{}

IGameRules* IGameRules::getDefault()
{
  return NEW NullGameRules();
}
