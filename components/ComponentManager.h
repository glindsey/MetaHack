#pragma once

#include "stdafx.h"

#include "components/ComponentMap.h"

#include "components/ComponentAppearance.h"
#include "components/ComponentPhysical.h"
#include "components/ComponentPosition.h"

// Forward declarations
class EntityId;
class GameState;

class ComponentManager final
{
public:
  ComponentManager();
  ComponentManager(json const& j);
  ~ComponentManager();

  void initialize();

  void populate(EntityId new_id, json const& jsonComponents);

  friend void from_json(json const& j, ComponentManager& obj);
  friend void to_json(json& j, ComponentManager const& obj);

  ComponentMap<ComponentAppearance> appearance;
  ComponentMap<ComponentPhysical> physical;
  ComponentMap<ComponentPosition> position;
};
