#include "stdafx.h"

#include "systems/Manager.h"

#include "components/ComponentManager.h"
#include "systems/SystemDirector.h"
#include "systems/SystemGrimReaper.h"
#include "systems/SystemJanitor.h"
#include "systems/SystemLighting.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemChoreographer.h"
#include "systems/SystemSenseSight.h"
#include "systems/SystemGeometry.h"
#include "systems/SystemTimekeeper.h"
#include "utilities/New.h"

namespace Systems
{

  Manager* Manager::s_instance = nullptr;

  Manager::Manager(GameState& gameState) :
    m_gameState{ gameState }
  {
    auto& components = m_gameState.components();

    // Set instance pointer.
    Assert("Systems", s_instance == nullptr, "tried to create more than one Manager instance at a time");
    s_instance = this;


    // Initialize systems.
    m_choreographer.reset(NEW Choreographer(components.globals));

    m_director.reset(NEW Director(m_gameState, *this));

    m_geometry.reset(NEW Geometry(components.globals,
                                  components.inventory,
                                  components.position));

    m_grimReaper.reset(NEW GrimReaper(components.globals));

    m_janitor.reset(NEW Janitor(components));

    m_lighting.reset(NEW Lighting(m_gameState,
                                  components.appearance,
                                  components.health,
                                  components.lightSource,
                                  components.position));

    m_narrator.reset(NEW Narrator(components.globals,
                                  components.bodyparts,
                                  components.category,
                                  components.gender,
                                  components.health,
                                  components.position,
                                  components.properName,
                                  components.quantity));

    m_senseSight.reset(NEW SenseSight(m_gameState,
                                      components.inventory,
                                      components.position,
                                      components.senseSight,
                                      components.spacialMemory));

    m_timekeeper.reset(NEW Timekeeper(components.globals));

    // Link system events.
    m_janitor->addObserver(*m_geometry, Janitor::EventEntityMarkedForDeletion::id);

    m_geometry->addObserver(*m_lighting, Geometry::EventEntityChangedMaps::id);
    m_geometry->addObserver(*m_lighting, Geometry::EventEntityMoved::id);

    m_geometry->addObserver(*m_senseSight, Geometry::EventEntityChangedMaps::id);
    m_geometry->addObserver(*m_senseSight, Geometry::EventEntityMoved::id);

  }

  Manager::~Manager()
  {
    // Dissolve links.
    m_geometry->removeAllObservers();

    s_instance = nullptr;
  }

  void Manager::runOneCycle()
  {
    m_director->doCycleUpdate();
    m_choreographer->doCycleUpdate();

    m_lighting->doCycleUpdate();
    m_senseSight->doCycleUpdate();
    //m_senseHearing->doCycleUpdate();
    //m_senseSmell->doCycleUpdate();
    //m_senseTouch->doCycleUpdate();
    m_geometry->doCycleUpdate();
    //m_physics->doCycleUpdate();
    //m_thermodynamics->doCycleUpdate();

    m_grimReaper->doCycleUpdate();
    m_janitor->doCycleUpdate();
    m_timekeeper->doCycleUpdate();
  }

  Manager & Manager::instance()
  {
    Assert("Systems", s_instance != nullptr, "tried to get non-existent Manager instance");

    return *(s_instance);
  }

} // end namespace Systems
