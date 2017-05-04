#include "stdafx.h"

#include "SystemManager.h"

#include "systems/SystemLighting.h"
#include "utilities/New.h"

SystemManager::SystemManager(ComponentManager& components) :
  m_components{ components }
{
  m_lighting.reset(NEW SystemLighting(m_components.lightSource, m_components.position));
}

SystemManager::~SystemManager()
{}

void SystemManager::runOneCycle()
{
  m_lighting->recalculate();
}

SystemLighting & SystemManager::lighting()
{
  return *m_lighting;
}
