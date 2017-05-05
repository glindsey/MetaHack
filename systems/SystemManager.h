#pragma once

#include <memory>

#include "components/ComponentManager.h"

// Forward declarations
class SystemLighting;
class SystemSenseSight;
class SystemSpacialRelationships;

class SystemManager final
{
public:
  SystemManager(ComponentManager& components);
  ~SystemManager();

  /// Run one cycle of all systems.
  void runOneCycle();

  // Get references to systems.
  SystemLighting& lighting() { return *m_lighting; }
  SystemSenseSight& senseSight() { return *m_senseSight; }
  SystemSpacialRelationships& spacial() { return *m_spacial; }

private:
  // System instances.
  std::unique_ptr<SystemLighting> m_lighting;
  std::unique_ptr<SystemSenseSight> m_senseSight;
  std::unique_ptr<SystemSpacialRelationships> m_spacial;

  /// Reference to the component manager.
  ComponentManager& m_components;
};
