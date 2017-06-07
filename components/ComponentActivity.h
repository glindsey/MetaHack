#pragma once

#include "actions/Action.h"
#include "actions/ActionQueue.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component that represents an entity that can perform actions.
  /// This doesn't necessarily mean the entity is choosing them of its own
  /// free will, only that it can perform them.
  /// The player character must have this component present.
  class ComponentActivity
  {
  public:

    friend void from_json(json const& j, ComponentActivity& obj);
    friend void to_json(json& j, ComponentActivity const& obj);

    Actions::ActionQueue& pendingActions();
    Actions::ActionQueue const& pendingActions() const;

    int busyTicks() const;
    void clearBusyTicks();
    void setBusyTicks(int value);
    void incBusyTicks(int value);
    void decBusyTicks(int value);

  protected:

  private:
    /// Queue of pending voluntary actions to be performed.
    Actions::ActionQueue m_pendingActions;

    /// Busy counter for this entity.
    int m_busyTicks = 0;
  };

} // end namespace Components
