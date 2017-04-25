#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that defines that this entity is capable of moving itself.
class ComponentMobility
{
public:

  friend void from_json(json const& j, ComponentMobility& obj);
  friend void to_json(json& j, ComponentMobility const& obj);

protected:

private:

};
