#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that allows the capability of sight.
class ComponentSenseSight
{
public:

  friend void from_json(json const& j, ComponentSenseSight& obj);
  friend void to_json(json& j, ComponentSenseSight const& obj);

protected:

private:

};
