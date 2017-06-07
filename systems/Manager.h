#pragma once

#include <memory>

#include "game/GameState.h"

namespace Systems
{

  // Forward declarations
  class SystemDirector;
  class SystemGrimReaper;
  class SystemJanitor;
  class SystemLighting;
  class SystemNarrator;
  class SystemPlayerHandler;
  class SystemSenseSight;
  class SystemSpacialRelationships;
  class SystemTimekeeper;

  class Manager final
  {
  public:
    Manager(GameState& gameState);
    ~Manager();

    /// Run one cycle of all systems.
    void runOneCycle();

    // Get references to systems.
    SystemDirector& director() { return *m_director; }
    SystemGrimReaper& grimReaper() { return *m_grimReaper; }
    SystemJanitor& janitor() { return *m_janitor; }
    SystemLighting& lighting() { return *m_lighting; }
    SystemNarrator& narrator() { return *m_narrator; }
    SystemPlayerHandler& playerHandler() { return *m_playerHandler; }
    SystemSenseSight& senseSight() { return *m_senseSight; }
    SystemSpacialRelationships& spacial() { return *m_spacial; }
    SystemTimekeeper& timekeeper() { return *m_timekeeper; }

    static Manager& instance();

  private:
    // System instances.
    std::unique_ptr<SystemDirector> m_director;
    std::unique_ptr<SystemGrimReaper> m_grimReaper;
    std::unique_ptr<SystemJanitor> m_janitor;
    std::unique_ptr<SystemLighting> m_lighting;
    std::unique_ptr<SystemNarrator> m_narrator;
    std::unique_ptr<SystemPlayerHandler> m_playerHandler;
    std::unique_ptr<SystemSenseSight> m_senseSight;
    std::unique_ptr<SystemSpacialRelationships> m_spacial;
    std::unique_ptr<SystemTimekeeper> m_timekeeper;

    /// Reference to the game state.
    GameState& m_gameState;

    /// Static pointer to the singleton instance of the Manager.
    static Manager* s_instance;
  };

} // end namespace Systems

#define SYSTEMS Systems::Manager::instance()