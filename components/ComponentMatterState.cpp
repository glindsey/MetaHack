#include "components/ComponentMatterState.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

std::ostream & operator<<(std::ostream & os, MatterState const & state)
{
  switch (state)
  {
  case MatterState::Void: os << "void"; break;
  case MatterState::Solid: os << "solid"; break;
  case MatterState::Liquid: os << "liquid"; break;
  case MatterState::Gaseous: os << "gaseous"; break;
  case MatterState::Plasma: os << "plasma"; break;
  case MatterState::Spectral: os << "spectral"; break;
  default: break;
  }

  return os;
}

std::ostream & operator<<(std::ostream & os, MatterSubstate const & state)
{
  switch (state)
  {
  case MatterSubstate::Granular: os << "granular"; break;
  case MatterSubstate::Slurry: os << "slurry"; break;
  case MatterSubstate::Viscous: os << "viscous"; break;
  case MatterSubstate::Fizzy: os << "fizzy"; break;
  default: break;
  }
  
  return os;
}

void from_json(json const& j, ComponentMatterState& obj)
{
  obj = ComponentMatterState();
  std::string state = j.get<std::string>();
  boost::to_lower(state);

  if (state.find("void") != std::string::npos) obj.m_state = MatterState::Void;
  else if (state.find("solid") != std::string::npos) obj.m_state = MatterState::Solid;
  else if (state.find("liquid") != std::string::npos) obj.m_state = MatterState::Liquid;
  else if (state.find("gas") != std::string::npos) obj.m_state = MatterState::Gaseous;
  else if (state.find("spectral") != std::string::npos) obj.m_state = MatterState::Spectral;

  if (state.find("granular") != std::string::npos) obj.m_substate = MatterSubstate::Granular;
  else if (state.find("slurry") != std::string::npos) obj.m_substate = MatterSubstate::Slurry;
  else if (state.find("viscous") != std::string::npos) obj.m_substate = MatterSubstate::Viscous;
  else if (state.find("fizzy") != std::string::npos) obj.m_substate = MatterSubstate::Fizzy;
}

void to_json(json& j, ComponentMatterState const& obj)
{
  std::stringstream state;
  std::stringstream substate;
  state << obj.m_substate << obj.m_state;
  j = state.str();
}

MatterState ComponentMatterState::state() const
{
  return m_state;
}

void ComponentMatterState::setState(MatterState state)
{
  m_state = state;
}

MatterSubstate ComponentMatterState::substate() const
{
  return m_substate;
}

void ComponentMatterState::setSubstate(MatterSubstate substate)
{
  m_substate = substate;
}

bool ComponentMatterState::isFluid()
{
  return m_state == MatterState::Liquid || 
    (m_state == MatterState::Solid && m_substate == MatterSubstate::Granular);
}
