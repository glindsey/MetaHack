#include "components/ComponentPhysical.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"

void from_json(json const& j, ComponentPhysical& obj)
{
  if (!j.is_object() || j.size() == 0)
  {
    obj = ComponentPhysical();
  }
  else
  {
    obj.m_mass = j["mass"];
    obj.m_quantity = j["quantity"];
    obj.m_volume = j["volume"];
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

void ComponentPhysical::setMass(int mass)
{
  m_mass = mass;
}

void ComponentPhysical::setQuantity(unsigned int quantity)
{
  m_quantity = quantity;
}

void ComponentPhysical::setVolume(unsigned int volume)
{
  m_volume = volume;
}

int ComponentPhysical::mass()
{
  return m_mass;
}

unsigned int ComponentPhysical::quantity()
{
  return m_quantity;
}

unsigned int ComponentPhysical::volume()
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

