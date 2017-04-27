#pragma once

#include "stdafx.h"

#include "components/ComponentMap.h"

#include "components/ComponentAppearance.h"
#include "components/ComponentBodyparts.h"
#include "components/ComponentGender.h"
#include "components/ComponentHealth.h"
#include "components/ComponentInventory.h"
#include "components/ComponentLightSource.h"
#include "components/ComponentLockable.h"
#include "components/ComponentMagicalBinding.h"
#include "components/ComponentMobility.h"
#include "components/ComponentOpenable.h"
#include "components/ComponentPhysical.h"
#include "components/ComponentPosition.h"
#include "components/ComponentSenseSight.h"
#include "components/ComponentSpacialMemory.h"

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

  void clone(EntityId original, EntityId newId);

  void populate(EntityId newId, json const& jsonComponents);

  friend void from_json(json const& j, ComponentManager& obj);
  friend void to_json(json& j, ComponentManager const& obj);

  ComponentMap<ComponentAppearance> appearance;
  ComponentMap<ComponentBodyparts> bodyparts;
  ComponentMap<std::string> category;
  ComponentMap<ComponentGender> gender;
  ComponentMap<ComponentHealth> health;
  ComponentMap<ComponentInventory> inventory;
  ComponentMap<ComponentLightSource> lightSource;
  ComponentMap<ComponentLockable> lockable;
  ComponentMap<ComponentMagicalBinding> magicalBinding;
  ComponentMap<ComponentMobility> mobility;
  ComponentMap<ComponentOpenable> openable;
  ComponentMap<ComponentPhysical> physical;
  ComponentMap<ComponentPosition> position;
  ComponentMap<ComponentSenseSight> senseSight;
  ComponentMap<ComponentSpacialMemory> spacialMemory;
};
