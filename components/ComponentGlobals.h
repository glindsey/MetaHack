#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"

/// Component containing global data.
class ComponentGlobals
{
public:

  friend void from_json(json const& j, ComponentGlobals& obj);
  friend void to_json(json& j, ComponentGlobals const& obj);

  EntityId player() const;
  void setPlayer(EntityId player);

protected:

private:
  EntityId m_player;

};
