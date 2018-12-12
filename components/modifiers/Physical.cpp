#include "components/modifiers/Physical.h"

#include "utilities/JSONUtils.h"

namespace Components
{
  namespace Modifiers
  {
    void from_json(json const& j, Physical& obj)
    {
      /// @todo WRITE ME
      //obj = ModifierPhysical();
      //JSONUtils::doIfPresent(j, "mass", [&](auto& value) { obj.m_mass = value; });
      //JSONUtils::doIfPresent(j, "volume", [&](auto& value) { obj.m_volume = value; });
    }

    void to_json(json& j, Physical const& obj)
    {
      /// @todo WRITE ME
      //j = json::object();
      //j["mass"] = obj.m_mass;
      //j["volume"] = obj.m_volume;
    }
  }
} // end namespace
