#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component indicating the state of matter of an entity.
/// If this component doesn't exist, the entity is assumed to be solid, and
/// will not change state due to temperature changes 
/// (e.g. freeze/melt/boil/condense).
class ComponentMatterState
{
public:

  friend void from_json(json const& j, ComponentMatterState& obj);
  friend void to_json(json& j, ComponentMatterState const& obj);

protected:

private:

};
