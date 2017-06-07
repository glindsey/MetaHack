#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component indicating the state of matter of an entity.
  /// If this component doesn't exist, the entity is assumed to be solid, and
  /// will not change state due to temperature changes 
  /// (e.g. freeze/melt/boil/condense).
  class ComponentMatterState
  {
  public:

    /// Enum outlining potential states of matter.
    /// Obviously not a 1:1 correspondence with actual states of matter, as this
    /// provides a few "fantasy" cases we may need.
    enum class State
    {
      Void,
      Solid,
      Liquid,
      Gaseous,
      Plasma,
      Spectral
    };

    friend std::ostream& operator<<(std::ostream& os, State const& state)
    {
      switch (state)
      {
      case State::Void: os << "void"; break;
      case State::Solid: os << "solid"; break;
      case State::Liquid: os << "liquid"; break;
      case State::Gaseous: os << "gaseous"; break;
      case State::Plasma: os << "plasma"; break;
      case State::Spectral: os << "spectral"; break;
      default: break;
      }

      return os;
    }

    /// Enum outlining substates of matter.
    enum class Substate
    {
      None,         ///< No substate specified
      Granular,     ///< Substate of solid; e.g. sand
      Slurry,       ///< Substate of liquid; includes mud, slush
      Viscous,      ///< Substate of liquid; e.g. honey
      Fizzy,        ///< Substate of liquid
    };

    friend std::ostream& operator<<(std::ostream& os, Substate const& state)
    {
      switch (state)
      {
      case Substate::Granular: os << "granular"; break;
      case Substate::Slurry: os << "slurry"; break;
      case Substate::Viscous: os << "viscous"; break;
      case Substate::Fizzy: os << "fizzy"; break;
      default: break;
      }

      return os;
    }

    friend void from_json(json const& j, ComponentMatterState& obj);
    friend void to_json(json& j, ComponentMatterState const& obj);

    State state() const;
    void setState(State state);

    Substate substate() const;
    void setSubstate(Substate substate);

    /// Returns true if matter can act as a fluid.
    bool isFluid();

  protected:

  private:
    /// State of matter.
    State m_state = State::Solid;

    /// Substate of matter, if any.
    Substate m_substate = Substate::None;
  };

} // end namespace Components
