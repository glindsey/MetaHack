#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Enum outlining potential states of matter.
/// Obviously not a 1:1 correspondence with actual states of matter, as this
/// provides a few "fantasy" cases we may need.
enum class MatterState
{
  Void,
  Solid,
  Liquid,
  Gaseous,
  Plasma,
  Spectral
};

std::ostream& operator<<(std::ostream& os, MatterState const& state);

/// Enum outlining substates of matter.
enum class MatterSubstate
{
  None,         ///< No substate specified
  Granular,     ///< Substate of solid; e.g. sand
  Slurry,       ///< Substate of liquid; includes mud, slush
  Viscous,      ///< Substate of liquid; e.g. honey
  Fizzy,        ///< Substate of liquid
};

std::ostream& operator<<(std::ostream& os, MatterSubstate const& state);

/// Component indicating the state of matter of an entity.
/// If this component doesn't exist, the entity is assumed to be solid, and
/// will not change state due to temperature changes 
/// (e.g. freeze/melt/boil/condense).
class ComponentMatterState
{
public:

  friend void from_json(json const& j, ComponentMatterState& obj);
  friend void to_json(json& j, ComponentMatterState const& obj);

  MatterState state() const;
  void setState(MatterState state);

  MatterSubstate substate() const;
  void setSubstate(MatterSubstate substate);

  /// Returns true if matter can act as a fluid.
  bool isFluid();

protected:

private:
  /// State of matter.
  MatterState m_state = MatterState::Solid;

  /// Substate of matter, if any.
  MatterSubstate m_substate = MatterSubstate::None;
};
