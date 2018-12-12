#pragma once

#include "components/modifiers/Base.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{
  namespace Modifiers
  {
    /// Modifier for physical dimensions.
    class Physical final : public Base
    {
    public:
      friend void from_json(json const& j, Physical& obj);
      friend void to_json(json& j, Physical const& obj);

      /// @todo WRITE ME

    protected:

    private:
    };
  } // end namespace Modifiers
} // end namespace Components
