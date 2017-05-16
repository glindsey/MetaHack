#include "stdafx.h"

#include "SystemManager.h"

#include "components/ComponentManager.h"
#include "systems/SystemLighting.h"
#include "systems/SystemSenseSight.h"
#include "systems/SystemSpacialRelationships.h"
#include "utilities/New.h"

SystemManager* SystemManager::s_instance = nullptr;

SystemManager::SystemManager(GameState& gameState) :
  m_gameState{ gameState }
{
  auto& components = m_gameState.components();

  // Set instance pointer.
  Assert("Systems", s_instance == nullptr, "tried to create more than one SystemManager instance at a time");
  s_instance = this;


  // Initialize systems.
  m_lighting.reset(NEW SystemLighting(components.appearance,
                                      components.health,
                                      components.lightSource, 
                                      components.position));

  m_senseSight.reset(NEW SystemSenseSight(components.inventory,
                                          components.position,
                                          components.senseSight,
                                          components.spacialMemory));

  m_spacial.reset(NEW SystemSpacialRelationships(components.inventory,
                                                 components.position));

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
