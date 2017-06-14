#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "types/BodyPart.h"

namespace Components
{

  /// Component that indicates an item is equippable on a bodypart.
  class ComponentEquippable
  {
  public:

    friend void from_json(json const& j, ComponentEquippable& obj);
    friend void to_json(json& j, ComponentEquippable const& obj);

    std::set<BodyPart> const& equippableOn() const;
  
  protected:

  private:
    std::set<BodyPart> m_equippableOn;

  };

} // end namespace