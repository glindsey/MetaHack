#pragma once

#include "types/Gender.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Gender and sex of an entity. Maybe also sexual preferences.
  class ComponentGender
  {
  public:

    friend void from_json(json const& j, ComponentGender& obj);
    friend void to_json(json& j, ComponentGender const& obj);

    /// Get this entity's gender.
    Gender& gender();
    Gender const& gender() const;

  protected:

  private:
    /// @todo Make this more flexible.
    Gender m_gender;

  };

} // end namespace Components
