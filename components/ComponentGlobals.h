#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"

namespace Components
{

  /// Component containing global data.
  class ComponentGlobals
  {
  public:

    friend void from_json(json const& j, Components::ComponentGlobals& obj);
    friend void to_json(json& j, Components::ComponentGlobals const& obj);

    EntityId player() const;
    void setPlayer(EntityId player);

  protected:

  private:
    EntityId m_player;

  };

} // end namespace Components
