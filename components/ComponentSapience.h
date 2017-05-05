#pragma once

#include <deque>

#include "actions/Action.h"

#include "json.hpp"
using json = ::nlohmann::json;

using ActionQueue = std::deque< std::unique_ptr<Actions::Action> >;

/// Component that represents an entity's ability to reason.
class ComponentSapience
{
public:

  friend void from_json(json const& j, ComponentSapience& obj);
  friend void to_json(json& j, ComponentSapience const& obj);

  ActionQueue& pendingActions();
  ActionQueue const& pendingActions() const;

protected:

private:
  /// Queue of pending voluntary actions to be performed.
  ActionQueue m_pendingActions;

};
