#include "stdafx.h"

#include "entity/EntityId.h"

#include "entity/EntityFactory.h"
#include "game/GameState.h"

/// Static constant referring to the Void entity.
EntityId const EntityId::Void = EntityId(0);

EntityId::EntityId() :
  m_id{ 0 }
{}

EntityId::EntityId(uint64_t id) :
  m_id{ id }
{}

EntityId::EntityId(std::string id) :
  m_id{ std::stoull(id) }
{}


void to_json(json& j, EntityId const& id)
{
  j = id.m_id;
}

void from_json(json const& j, EntityId& id)
{
  id.m_id = j.get<uint64_t>();
}

EntityId::operator bool() const
{
  return m_id != 0;
}

bool EntityId::operator!() const
{
  return !operator bool();
}

/// Convert to Lua integer.
/// @warning If for some crazy reason m_id is somehow larger than
///          2^53, this will NOT WORK PROPERLY due to precision loss.
///          This is because LuaJIT stores all numbers as doubles.
EntityId::operator lua_Integer() const
{
  return static_cast<lua_Integer>(m_id);
}

EntityId::operator uint64_t() const
{
  return m_id;
}

EntityId::operator std::string() const
{
  return std::to_string(m_id);
}

bool EntityId::operator<(EntityId const& other) const
{
  return (this->m_id < other.m_id);
}

bool EntityId::operator<=(EntityId const& other) const
{
  return (this->m_id <= other.m_id);
}

bool EntityId::operator>(EntityId const& other) const
{
  return !operator<=(other);
}

bool EntityId::operator>=(EntityId const& other) const
{
  return !operator<(other);
}

bool EntityId::operator==(EntityId const& other) const
{
  return (this->m_id == other.m_id);
}

bool EntityId::operator==(uint64_t const& other) const
{
  return (this->m_id == other);
}

bool EntityId::operator!=(EntityId const& other) const
{
  return !operator==(other);
}

bool EntityId::operator!=(uint64_t const& other) const
{
  return !operator==(other);
}

EntityId& EntityId::operator++()
{
  ++m_id;
  return *this;
}

EntityId EntityId::operator++(int)
{
  EntityId tmp(*this);
  operator++();
  return tmp;
}

EntityId& EntityId::operator--()
{
  --m_id;
  return *this;
}

EntityId EntityId::operator--(int)
{
  EntityId tmp(*this);
  operator--();
  return tmp;
}

std::ostream& operator<<(std::ostream& stream, EntityId const& entity)
{
  stream << entity.m_id;
  return stream;
}
