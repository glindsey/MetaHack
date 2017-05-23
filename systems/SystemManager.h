#pragma once

#include <memory>

#include "game/GameState.h"

// Forward declarations
class SystemLighting;
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
  SystemLighting& lighting() { return *m_lighting; }
  SystemPlayerHandler& playerHandler() { return *m_playerHandler; }
  SystemSenseSight& senseSight() { return *m_senseSight; }
  SystemSpacialRelationships& spacial() { return *m_spacial; }
  SystemTimekeeper& timekeeper() { return *m_timekeeper; }

  static SystemManager& instance();

private:
  // System instances.
  std::unique_ptr<SystemLighting> m_lighting;
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