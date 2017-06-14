#pragma once

#include "components/ComponentMap.h"

#include "components/ComponentGlobals.h"
#include "components/ComponentActivity.h"
#include "components/ComponentAppearance.h"
#include "components/ComponentBodyparts.h"
#include "components/ComponentCombustible.h"
#include "components/ComponentCorrodible.h"
#include "components/ComponentDigestiveSystem.h"
#include "components/ComponentEquippable.h"
#include "components/ComponentGender.h"
#include "components/ComponentHealth.h"
#include "components/ComponentInventory.h"
#include "components/ComponentLightSource.h"
#include "components/ComponentLockable.h"
#include "components/ComponentMagicalBinding.h"
#include "components/ComponentMaterialFlags.h"
#include "components/ComponentMatterState.h"
#include "components/ComponentMobility.h"
#include "components/ComponentOpenable.h"
#include "components/ComponentPhysical.h"
#include "components/ComponentPosition.h"
#include "components/ComponentSapience.h"
#include "components/ComponentSenseSight.h"
#include "components/ComponentSpacialMemory.h"

// Forward declarations
class EntityId;
class GameState;

namespace Components
{

  class ComponentManager final
  {
  public:
    ComponentManager(GameState& gameState);
    ComponentManager(GameState& gameState, json const& j);
    ~ComponentManager();

    void initialize();

    void clone(EntityId original, EntityId newId);

    void erase(EntityId id);

    void populate(EntityId newId, json const& jsonComponents);

    json toJSON();

    ComponentGlobals globals;
    ComponentMap<ComponentActivity> activity;
    ComponentMap<ComponentAppearance> appearance;
    ComponentMap<ComponentBodyparts> bodyparts;
    ComponentMap<std::string> category;
    ComponentMap<ComponentCombustible> combustible;
    ComponentMap<ComponentCorrodible> corrodible;
    ComponentMap<ComponentDigestiveSystem> digestiveSystem;
    ComponentMap<ComponentEquippable> equippable;
    ComponentMap<ComponentGender> gender;
    ComponentMap<ComponentHealth> health;
    ComponentMap<ComponentInventory> inventory;
    ComponentMap<ComponentLightSource> lightSource;
    ComponentMap<ComponentLockable> lockable;
    ComponentMap<ComponentMagicalBinding> magicalBinding;
    ComponentMap<ComponentMaterialFlags> materialFlags;
    ComponentMap<ComponentMatterState> matterState;
    ComponentMap<ComponentMobility> mobility;
    ComponentMap<ComponentOpenable> openable;
    ComponentMap<ComponentPhysical> physical;
    ComponentMap<ComponentPosition> position;
    ComponentMap<unsigned int> quantity;
    ComponentMap<std::string> properName;
    ComponentMap<ComponentSapience> sapience;
    ComponentMap<ComponentSenseSight> senseSight;
    ComponentMap<ComponentSpacialMemory> spacialMemory;

  private:
    /// Reference to parent GameState instance.
    GameState& m_gameState;
  };

} // end namespace Components
