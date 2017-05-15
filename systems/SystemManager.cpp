#include "stdafx.h"

#include "SystemManager.h"

#include "systems/SystemLighting.h"
#include "systems/SystemSenseSight.h"
#include "systems/SystemSpacialRelationships.h"
#include "utilities/New.h"

SystemManager* SystemManager::s_instance = nullptr;

SystemManager::SystemManager(ComponentManager& components) :
  m_components{ components }
{
  // Set instance pointer.
  Assert("Systems", s_instance == nullptr, "tried to create more than one SystemManager instance at a time");
  s_instance = this;


  // Initialize systems.
  m_lighting.reset(NEW SystemLighting(m_components.appearance,
                                      m_components.health,
                                      m_components.lightSource, 
                                      m_components.position));

  m_senseSight.reset(NEW SystemSenseSight(m_components.inventory,
                                          m_components.position,
                                          m_components.senseSight,
                                          m_components.spacialMemory));

  m_spacial.reset(NEW SystemSpacialRelationships(m_components.inventory,
                                                 m_components.position));

  // Link system events.
  m_spacial->addObserver(*m_lighting, SystemSpacialRelationships::EventEntityChangedMaps::id);
  m_spacial->addObserver(*m_lighting, SystemSpacialRelationships::EventEntityMoved::id);

  m_spacial->addObserver(*m_senseSight, SystemSpacialRelationships::EventEntityChangedMaps::id);
  m_spacial->addObserver(*m_senseSight, SystemSpacialRelationships::EventEntityMoved::id);

}

SystemManager::~SystemManager()
{
  // Dissolve links.
  m_spacial->removeAllObservers();

  s_instance = nullptr;
}

void SystemManager::runOneCycle()
{
  m_lighting->doCycleUpdate();
  m_senseSight->doCycleUpdate();
  m_spacial->doCycleUpdate();
}

SystemManager & SystemManager::instance()
{
  Assert("Systems", s_instance != nullptr, "tried to get non-existent SystemManager instance");

  return *(s_instance);
}
