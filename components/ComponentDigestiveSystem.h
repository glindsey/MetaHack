#pragma once

#include "components/ComponentMaterialFlags.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component that indicates that this entity eats and/or drinks.
  /// @note Could also deal with... elimination... but probably not. There's
  ///       such a thing as being *too* realistic.
  class ComponentDigestiveSystem
  {
  public:

    friend void from_json(json const& j, ComponentDigestiveSystem& obj);
    friend void to_json(json& j, ComponentDigestiveSystem const& obj);

    bool needsMouth() const;

  protected:

  private:
    /// Flags indicating what sorts of matter the entity can feed on.
    /// An entity must have at least one of these flags set to be considered
    /// "edible" by this entity.
    ComponentMaterialFlags m_allowedFoods;

    /// Flags indicating what sorts of matter the entity prefers to feed on.
    ComponentMaterialFlags m_preferredFoods;

    /// Flags indicating what sorts of matter are taboo to feed on.
    /// An entity containing one or more of these flags will only be eaten as
    /// an absolute last resort.
    ComponentMaterialFlags m_tabooFoods;

    /// Indicates whether entity needs a functioning and accessible mouth to
    /// eat/drink. Defaults to true if absent.
    bool m_needsMouth = true;
  };

} // end namespace Components
