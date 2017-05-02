#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "game/GameState.h"
#include "map/MapId.h"
#include "types/Color.h"

/// Component that describes physical appearance of the entity:
/// right now this is just R/G/B opacity.
class ComponentAppearance final
{
public:
  ComponentAppearance();
  ComponentAppearance(Color opacity);
  ~ComponentAppearance();

  friend void from_json(json const& j, ComponentAppearance& obj);
  friend void to_json(json& j, ComponentAppearance const& obj);

  /// Get this entity's opacity.
  Color& opacity();
  Color const& opacity() const;

protected:

private:
  /// @todo Make this modifiable?
  Color m_opacity;
};

