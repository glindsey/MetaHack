#include "stdafx.h"

#include "components/ComponentMatterState.h"

#include <boost/algorithm/string.hpp>

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{
  void from_json(json const& j, ComponentMatterState& obj)
  {
    obj = ComponentMatterState();
    std::string state = j.get<std::string>();
    boost::to_lower(state);

    if (state.find("void") != std::string::npos) obj.m_state = ComponentMatterState::State::Void;
    else if (state.find("solid") != std::string::npos) obj.m_state = ComponentMatterState::State::Solid;
    else if (state.find("liquid") != std::string::npos) obj.m_state = ComponentMatterState::State::Liquid;
    else if (state.find("gas") != std::string::npos) obj.m_state = ComponentMatterState::State::Gaseous;
    else if (state.find("spectral") != std::string::npos) obj.m_state = ComponentMatterState::State::Spectral;

    if (state.find("granular") != std::string::npos) obj.m_substate = ComponentMatterState::Substate::Granular;
    else if (state.find("slurry") != std::string::npos) obj.m_substate = ComponentMatterState::Substate::Slurry;
    else if (state.find("viscous") != std::string::npos) obj.m_substate = ComponentMatterState::Substate::Viscous;
    else if (state.find("fizzy") != std::string::npos) obj.m_substate = ComponentMatterState::Substate::Fizzy;
  }

  void to_json(json& j, ComponentMatterState const& obj)
  {
    std::stringstream state;
    std::stringstream substate;
    state << obj.m_substate << obj.m_state;
    j = state.str();
  }

  ComponentMatterState::State ComponentMatterState::state() const
  {
    return m_state;
  }

  void ComponentMatterState::setState(ComponentMatterState::State state)
  {
    m_state = state;
  }

  ComponentMatterState::Substate ComponentMatterState::substate() const
  {
    return m_substate;
  }

  void ComponentMatterState::setSubstate(ComponentMatterState::Substate substate)
  {
    m_substate = substate;
  }

  bool ComponentMatterState::isFluid()
  {
    return m_state == State::Liquid || (m_state == State::Solid && m_substate == Substate::Granular);
  }

} // end namespace