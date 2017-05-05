#include "stdafx.h"

#include "SystemManager.h"

#include "systems/SystemLighting.h"
#include "systems/SystemSenseSight.h"
#include "systems/SystemSpacialRelationships.h"
#include "utilities/New.h"

SystemManager::SystemManager(ComponentManager& components) :
  m_components{ components }
{
  m_lighting.reset(NEW SystemLighting(m_components.appearance,
                                      m_components.health,
                                      m_components.lightSource, 
                                      m_components.position));

  m_senseSight.reset(NEW SystemSenseSight(m_components.inventory,
                                          m_components.position,
                                          m_components.senseSight));

  m_spacial.reset(NEW SystemSpacialRelationships(m_components.inventory,
                                                 m_components.position));
}

SystemManager::~SystemManager()
{}

void SystemManager::runOneCycle()
{
  m_lighting->recalculate();
  m_senseSight->recalculate();
  m_spacial->recalculate();
}