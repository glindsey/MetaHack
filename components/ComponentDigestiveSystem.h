#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Template to use for making new components.
class ComponentDigestiveSystem
{
public:

  friend void from_json(json const& j, ComponentDigestiveSystem& obj);
  friend void to_json(json& j, ComponentDigestiveSystem const& obj);

protected:

private:

};
