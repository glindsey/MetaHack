#include "stdafx.h"

#include "components/ComponentInventory.h"

#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "game/GameState.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"

ComponentInventory::ComponentInventory()
{
}

ComponentInventory::~ComponentInventory()
{
  //dtor
}

void from_json(json const& j, ComponentInventory& obj)
{
  obj.m_entities.clear();

  if (j.is_object() && j.size() != 0)
  {
    int maxSize = j["max-size"].get<int>();
    if (maxSize == -1)
    {
      obj.m_maxSize = SIZE_MAX;
    }
    else
    {
      obj.m_maxSize = static_cast<unsigned int>(maxSize);
    }

    if (j.count("items") != 0)
    {
      json const& items = j["items"];
      for (auto citer = items.cbegin(); citer != items.cend(); ++citer)
      {
        obj.m_entities[citer.key()] = citer.value();
      }
    }
  }
}

void to_json(json& j, ComponentInventory const& obj)
{
  j = json::object();

  j["max-size"] = obj.m_maxSize;
  json& items = j["items"];

  for (auto citer = obj.m_entities.cbegin(); citer != obj.m_entities.cend(); ++citer)
  {
    items[citer->first] = citer->second;
  }
}

/// @todo Handle max inventory size.
bool ComponentInventory::add(EntityId entity)
{
  // If entity is Mu, exit returning false.
  if (entity == EntityId::Mu())
  {
    return false;
  }

  // If the entity is the player, it goes into slot 0.
  if (entity == GAME.getPlayer())
  {
    if (m_entities.count(InventorySlot::Zero) != 0)
    {
      /// @todo Move anything in this slot to a new slot.  This will be required
      ///       if it's possible to change the ID of the player.
      CLOG(ERROR, "Inventory") << "slot 0 of inventory already contains the player";
    }
    m_entities[InventorySlot::Zero] = entity;
    return true;
  }

  auto found_thing_id = find(entity);

  if (found_thing_id == m_entities.cend())
  {
    for (InventorySlot slot = InventorySlot::Min; slot < InventorySlot::Max; ++slot)
    {
      if (m_entities.count(slot) == 0)
      {
        m_entities[slot] = entity;
        consolidateItems();
        return true;
      }
    }
  }

  return false;
}

void ComponentInventory::clear()
{
  m_entities.clear();
}

size_t const& ComponentInventory::maxSize() const
{
  return m_maxSize;
}

size_t ComponentInventory::count()
{
  return m_entities.size();
}

EntityMap::iterator ComponentInventory::begin()
{
  return std::begin(m_entities);
}

EntityMap::iterator ComponentInventory::end()
{
  return std::end(m_entities);
}

EntityMap::const_iterator ComponentInventory::cbegin()
{
  return m_entities.cbegin();
}

EntityMap::const_iterator ComponentInventory::cend()
{
  return m_entities.cend();
}

void ComponentInventory::consolidateItems()
{
  auto firstIter = std::begin(m_entities);
  while (firstIter != std::end(m_entities))
  {
    ++firstIter;
    auto secondIter = firstIter;
    --firstIter;

    while (secondIter != std::end(m_entities))
    {
      EntityId firstEntity = firstIter->second;
      EntityId secondEntity = secondIter->second;

      if (!COMPONENTS.quantity.existsFor(firstEntity) || !COMPONENTS.quantity.existsFor(secondEntity)) return;

      if (firstEntity->can_merge_with(secondEntity))
      {
        auto firstQuantity = COMPONENTS.quantity[firstEntity];
        auto secondQuantity = COMPONENTS.quantity[secondEntity];
        COMPONENTS.quantity[firstEntity] = firstQuantity + secondQuantity;
        COMPONENTS.quantity[secondEntity] = 0;

        auto secondIterCopy = secondIter;
        --secondIter;
        m_entities.erase(secondIterCopy);
      }
      ++secondIter;
    }
    ++firstIter;
  }
}

bool ComponentInventory::contains(EntityId entity)
{
  if (GAME.entities().exists(entity) == false) return false;

  return (find(entity) != m_entities.cend());
}

bool ComponentInventory::contains(InventorySlot slot)
{
  return (m_entities.count(slot) != 0);
}

InventorySlot ComponentInventory::operator[](EntityId entity)
{
  if (GAME.entities().exists(entity) == false) return InventorySlot::Invalid;

  auto iter = find(entity);

  if (iter != m_entities.cend())
  {
    return iter->first;
  }

  return InventorySlot::Invalid;
}

EntityId ComponentInventory::operator[](InventorySlot slot)
{
  return (m_entities.at(slot));
}

EntityId ComponentInventory::split(EntityId entity, unsigned int targetQuantity)
{
  EntityId targetEntity = EntityId::Mu();

  if (targetQuantity > 0 && COMPONENTS.quantity.existsFor(entity))
  {
    auto iter = find(entity);

    if (iter != m_entities.cend())
    {
      EntityId sourceEntity = iter->second;
      unsigned int sourceQuantity = COMPONENTS.quantity[sourceEntity];
      if (targetQuantity < sourceQuantity)
      {
        targetEntity = GAME.entities().clone(sourceEntity);
        COMPONENTS.quantity[sourceEntity] = sourceQuantity - targetQuantity;
        COMPONENTS.quantity[targetEntity] = targetQuantity;
      }
    }
  }

  return targetEntity;
}

EntityId ComponentInventory::remove(InventorySlot slot)
{
  EntityId removed_thing;
  if (m_entities.count(slot) != 0)
  {
    removed_thing = m_entities[slot];
    m_entities.erase(slot);
  }
  return removed_thing;
}

EntityId ComponentInventory::remove(EntityId entity)
{
  EntityId removed_thing;

  auto iter = find(entity);
  if (iter != m_entities.cend())
  {
    removed_thing = iter->second;
    m_entities.erase(iter);
  }
  return removed_thing;
}

EntityId ComponentInventory::get_largest_thing()
{
  auto iter_largest = m_entities.cbegin();

  for (EntityMap::const_iterator iter = m_entities.cbegin();
       iter != m_entities.cend(); ++iter)
  {
    if (isSmallerThan(iter_largest->second, iter->second))
    {
      iter_largest = iter;
    }
  }
  return iter_largest->second;
}

EntityId ComponentInventory::getEntity()
{
  auto iter =
    find_if([&](const EntityPair& thing_pair)
  {
    EntityId entity = thing_pair.second;
    return (COMPONENTS.health.existsFor(entity) && COMPONENTS.health[entity].hp() > 0);
  });

  if (iter != m_entities.cend())
  {
    return iter->second;
  }
  else
  {
    return EntityId::Mu();
  }
}

bool ComponentInventory::canContain(EntityId entity)
{
  if ((m_maxSize == 0) || (m_entities.size() > m_maxSize))
  {
    return false;
  }
  else
  {
    /// @todo Reimplement me
    //return call_lua_function("can_contain", entity, true);
  }
  return true;
}

EntityMap::iterator ComponentInventory::find_if(std::function<bool(EntityPair const&)> functor)
{
  EntityMap::iterator iter =
    std::find_if(m_entities.begin(), m_entities.end(), functor);
  return iter;
}

EntityMap::iterator ComponentInventory::find(EntityId target_id)
{
  EntityMap::iterator iter =
    std::find_if(
      m_entities.begin(),
      m_entities.end(),
      [&](EntityPair const& thing_pair)
  {
    return thing_pair.second == target_id;
  });

  return iter;
}

bool ComponentInventory::isSmallerThan(EntityId a, EntityId b)
{
  if ((a == EntityId::Mu()) || (b == EntityId::Mu())) return false;
  if (!COMPONENTS.physical.existsFor(a) || !COMPONENTS.physical.existsFor(b)) return false;
  auto firstQuantity = COMPONENTS.quantity.valueOr(a, 1);
  auto secondQuantity = COMPONENTS.quantity.valueOr(b, 1);

  auto firstVolume = COMPONENTS.physical[a].volume().value() * firstQuantity;
  auto secondVolume = COMPONENTS.physical[b].volume().value() * secondQuantity;

  return (firstVolume < secondVolume);
}