#pragma once

#include <memory>

#include "components/ComponentManager.h"

// Forward declarations
class SystemLighting;

class SystemManager final
{
public:
  SystemManager(ComponentManager& components);
  ~SystemManager();

  /// Run one cycle of all systems.
  void run();

  // Get references to systems.
  SystemLighting& lighting();

private:
  /// Lighting system instance.
  std::unique_ptr<SystemLighting> m_lighting;

  /// Reference to the component manager.
  ComponentManager& m_components;
};
