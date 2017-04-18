#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Template to use for making new components.
class Component
{
public:

  friend void from_json(json const& j, Component& obj);
  friend void to_json(json& j, Component const& obj);

protected:

private:

};
