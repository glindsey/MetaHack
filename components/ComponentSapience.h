#pragma once

#include <boost/ptr_container/ptr_deque.hpp>

#include "actions/Action.h"

#include "json.hpp"
using json = ::nlohmann::json;

using ActionDeque = boost::ptr_deque< Actions::Action >;

/// Component that represents an entity's ability to reason.
class ComponentSapience
{
public:

  friend void from_json(json const& j, ComponentSapience& obj);
  friend void to_json(json& j, ComponentSapience const& obj);

  ActionDeque& pendingActions();
  ActionDeque const& pendingActions() const;

protected:

private:
  /// Queue of pending voluntary actions to be performed.
  ActionDeque m_pendingActions;

};
