#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "types/ModifiableBool.h"
#include "types/ModifiableInt.h"

#include <vector>

namespace Components
{

  /// Component that indicates an entity can rust, corrode, etc.
  class ComponentCorrodible
  {
  public:

    friend void from_json(json const& j, ComponentCorrodible& obj);
    friend void to_json(json& j, ComponentCorrodible const& obj);

    ModifiableBool& corrodesForever();
    ModifiableBool const& corrodesForever() const;

    int corrosionAmount() const;
    void setCorrosionAmount(int burnAmount);
    void incCorrosionAmount(int incAmount);
    void decCorrosionAmount(int decAmount);

    ModifiableInt& corrosionSpeed();
    ModifiableInt const& corrosionSpeed() const;

    std::vector<std::string> const & corrosionResults() const;

  protected:

  private:
    /// Flag indicating whether the entity corrodes forever, e.g. it can get
    /// rusty/patinated/etc. but doesn't fall apart completely.
    ModifiableBool m_corrodesForever;

    /// Value indicating how corroded the entity is.
    /// 0 indicates a pristine entity.
    /// When the value hits 100 * mass, it is destroyed and replaced by the contents
    /// of `m_corrosionResults`, unless `m_corrodesForever` is true, in which case the 
    /// value will stop increasing at 50 * mass.
    int m_corrosionAmount;

    /// Value indicating how fast the entity corrodes.
    /// This value, multiplied by the corroder's corrosion speed, is added to 
    /// `m_corrosionAmount` on each clock tick.
    ModifiableInt m_corrosionSpeed;

    /// Vector of entity types created when this entity is fully corroded.
    std::vector<std::string> m_corrosionResults;
  };

} // end namespace Components
