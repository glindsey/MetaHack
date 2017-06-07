#pragma once

#include <memory>

#include "game/GameState.h"

namespace Systems
{

  // Forward declarations
  class Director;
  class GrimReaper;
  class Janitor;
  class Lighting;
  class Narrator;
  class Choreographer;
  class SenseSight;
  class Geometry;
  class Timekeeper;

  class Manager final
  {
  public:
    Manager(GameState& gameState);
    ~Manager();

    /// Run one cycle of all systems.
    void runOneCycle();

    // Get references to systems.
    Choreographer& choreographer() { return *m_choreographer; }
    Director& director() { return *m_director; }
    Geometry& geometry() { return *m_geometry; }
    GrimReaper& grimReaper() { return *m_grimReaper; }
    Janitor& janitor() { return *m_janitor; }
    Lighting& lighting() { return *m_lighting; }
    Narrator& narrator() { return *m_narrator; }
    SenseSight& senseSight() { return *m_senseSight; }
    Timekeeper& timekeeper() { return *m_timekeeper; }

    static Manager& instance();

  private:
    // System instances.
    std::unique_ptr<Choreographer> m_choreographer;
    std::unique_ptr<Director> m_director;
    std::unique_ptr<Geometry> m_geometry;
    std::unique_ptr<GrimReaper> m_grimReaper;
    std::unique_ptr<Janitor> m_janitor;
    std::unique_ptr<Lighting> m_lighting;
    std::unique_ptr<Narrator> m_narrator;
    std::unique_ptr<SenseSight> m_senseSight;
    std::unique_ptr<Timekeeper> m_timekeeper;

    /// Reference to the game state.
    GameState& m_gameState;

    /// Static pointer to the singleton instance of the Manager.
    static Manager* s_instance;
  };

} // end namespace Systems

#define SYSTEMS Systems::Manager::instance()