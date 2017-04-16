#pragma once

#include "stdafx.h"

#include "components/ComponentMap.h"

#include "components/ComponentPosition.h"

// Forward declarations
class GameState;

class ComponentManager final
{
public:
  ComponentManager();
  ~ComponentManager();

  friend void from_json(json const& j, ComponentManager& obj);
  friend void to_json(json& j, ComponentManager const& obj);

  /// Positions map
  ComponentMap<ComponentPosition> position;
};