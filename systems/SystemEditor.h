#pragma once

#include "components/ComponentGlobals.h"
#include "components/ComponentManager.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

namespace Systems
{
  /// System that handles applying/updating/deleting modifiers from components.
  class Editor : public CRTP<Editor>
  {
  public:
    Editor(Components::ComponentGlobals& globals,
           Components::ModifiersGroup& modifiers);

    virtual ~Editor();

    /// Recalculate whatever needs recalculating.
    virtual void doCycleUpdate() override;

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
    Components::ComponentGlobals& m_globals;
    Components::ModifiersGroup& m_modifiers;
  };

} // end namespace Systems
