#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that represents an entity's ability to reason.
class ComponentSapience
{
public:

  friend void from_json(json const& j, ComponentSapience& obj);
  friend void to_json(json& j, ComponentSapience const& obj);

protected:

private:

};
