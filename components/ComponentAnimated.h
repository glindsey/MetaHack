#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component that indicates this entity has multiple tiles to choose from.
  /// Although it is called "ComponentAnimated", it doesn't necessarily mean the
  /// entity animates per frame, just that its appearance varies based on a
  /// function of something.
  class ComponentAnimated
  {
  public:

    friend void from_json(json const& j, ComponentAnimated& obj);
    friend void to_json(json& j, ComponentAnimated const& obj);

  protected:

  private:

  };

} // end namespace Components
