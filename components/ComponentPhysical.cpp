#include "components/ComponentPhysical.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentPhysical& obj)
{
  obj = ComponentPhysical();

  if (j.is_object() && j.size() != 0)
  {
    JSONUtils::setIfPresent(obj.m_mass, j, "mass");
    JSONUtils::setIfPresent(obj.m_quantity, j, "quantity");
    JSONUtils::setIfPresent(obj.m_volume, j, "volume");
  }
}

void to_json(json& j, ComponentPhysical const& obj)
{
  j = json::object();
  j["mass"] = obj.m_mass;
  j["quantity"] = obj.m_quantity;
  j["volume"] = obj.m_volume;
}

ComponentPhysical::ComponentPhysical()
  : m_mass{ 1000 }, m_quantity{ 1 }, m_volume{ 1000 }
{}

ComponentPhysical::ComponentPhysical(int mass, unsigned int quantity, unsigned int volume)
  : m_mass{ mass }, m_quantity{ quantity }, m_volume{ volume }
{}

ComponentPhysical::~ComponentPhysical()
{}

int& ComponentPhysical::mass()
{
  return m_mass;
}

int const& ComponentPhysical::mass() const
{
  return m_mass;
}

unsigned int& ComponentPhysical::quantity()
{
  return m_quantity;
}

unsigned int const& ComponentPhysical::quantity() const
{
  return m_quantity;
}

unsigned int& ComponentPhysical::volume()
{
  return m_volume;
}

unsigned int const& ComponentPhysical::volume() const
{
  return m_volume;
}

int ComponentPhysical::totalMass()
{
  return m_mass * static_cast<int>(m_quantity);
}

int ComponentPhysical::totalVolume()
{
  return m_volume * static_cast<int>(m_quantity);
}

