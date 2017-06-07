#pragma once

#include <memory>

#include "game/GameState.h"

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

class SystemManager final
{
public:
  SystemManager(GameState& gameState);
  ~SystemManager();

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

  static SystemManager& instance();

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

  /// Static pointer to the singleton instance of the SystemManager.
  static SystemManager* s_instance;
};

#define SYSTEMS SystemManager::instance()