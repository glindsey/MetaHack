#include "stdafx.h"

#include "SystemManager.h"

#include "components/ComponentManager.h"
#include "systems/SystemGrimReaper.h"
#include "systems/SystemJanitor.h"
#include "systems/SystemLighting.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemPlayerHandler.h"
#include "systems/SystemSenseSight.h"
#include "systems/SystemSpacialRelationships.h"
#include "systems/SystemTimekeeper.h"
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
  m_grimReaper.reset(NEW SystemGrimReaper(components.globals));

  m_janitor.reset(NEW SystemJanitor(components));

  m_lighting.reset(NEW SystemLighting(m_gameState,
                                      components.appearance,
                                      components.health,
                                      components.lightSource, 
                                      components.position));

  m_narrator.reset(NEW SystemNarrator(components.globals,
                                      components.bodyparts,
                                      components.category,
                                      components.gender,
                                      components.health,
                                      components.position,
                                      components.properName,
                                      components.quantity));

  m_playerHandler.reset(NEW SystemPlayerHandler(components.globals));

  m_senseSight.reset(NEW SystemSenseSight(m_gameState,
                                          components.inventory,
                                          components.position,
                                          components.senseSight,
                                          components.spacialMemory));

  m_spacial.reset(NEW SystemSpacialRelationships(components.globals,
                                                 components.inventory,
                                                 components.position));

  m_timekeeper.reset(NEW SystemTimekeeper(components.globals));

  // Link system events.
  m_janitor->addObserver(*m_spacial, SystemJanitor::EventEntityMarkedForDeletion::id);

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
  m_playerHandler->doCycleUpdate();

  m_lighting->doCycleUpdate();
  m_senseSight->doCycleUpdate();
  m_spacial->doCycleUpdate();

  m_grimReaper->doCycleUpdate();
  m_janitor->doCycleUpdate();
  m_timekeeper->doCycleUpdate();  
}

SystemManager & SystemManager::instance()
{
  Assert("Systems", s_instance != nullptr, "tried to get non-existent SystemManager instance");

  return *(s_instance);
}
