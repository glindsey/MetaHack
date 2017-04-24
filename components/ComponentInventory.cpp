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
  obj.m_things.clear();

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
        obj.m_things[citer.key()] = citer.value();
      }
    }
  }
}

void to_json(json& j, ComponentInventory const& obj)
{
  j = json::object();

  j["max-size"] = obj.m_maxSize;
  json& items = j["items"];

  for (auto citer = obj.m_things.cbegin(); citer != obj.m_things.cend(); ++citer)
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
    if (m_things.count(InventorySlot::Zero) != 0)
    {
      /// @todo Move anything in this slot to a new slot.  This will be required
      ///       if it's possible to change the ID of the player.
      CLOG(ERROR, "Inventory") << "slot 0 of inventory already contains the player";
    }
    m_things[InventorySlot::Zero] = entity;
    return true;
  }

  auto found_thing_id = find(entity);

  if (found_thing_id == m_things.cend())
  {
    for (InventorySlot slot = InventorySlot::Min; slot < InventorySlot::Max; ++slot)
    {
      if (m_things.count(slot) == 0)
      {
        m_things[slot] = entity;
        consolidateItems();
        return true;
      }
    }
  }

  return false;
}

void ComponentInventory::clear()
{
  m_things.clear();
}

size_t const& ComponentInventory::maxSize() const
{
  return m_maxSize;
}

size_t ComponentInventory::count()
{
  return m_things.size();
}

EntityMap::iterator ComponentInventory::begin()
{
  return std::begin(m_things);
}

EntityMap::iterator ComponentInventory::end()
{
  return std::end(m_things);
}

EntityMap::const_iterator ComponentInventory::cbegin()
{
  return m_things.cbegin();
}

EntityMap::const_iterator ComponentInventory::cend()
{
  return m_things.cend();
}

void ComponentInventory::consolidateItems()
{
  auto first_iter = std::begin(m_things);
  while (first_iter != std::end(m_things))
  {
    ++first_iter;
    auto second_iter = first_iter;
    --first_iter;

    while (second_iter != std::end(m_things))
    {
      EntityId first_thing = first_iter->second;
      EntityId second_thing = second_iter->second;

      if (first_thing->can_merge_with(second_thing))
      {
        auto first_quantity = COMPONENTS.physical.valueOrDefault(first_thing).quantity();
        auto second_quantity = COMPONENTS.physical.valueOrDefault(second_thing).quantity();
        COMPONENTS.physical[first_thing].quantity() = first_quantity + second_quantity;
        COMPONENTS.physical[second_thing].quantity() = 0;

        auto second_iter_copy = second_iter;
        --second_iter;
        m_things.erase(second_iter_copy);
      }
      ++second_iter;
    }
    ++first_iter;
  }
}

bool ComponentInventory::contains(EntityId entity)
{
  if (GAME.entities().exists(entity) == false) return false;

  return (find(entity) != m_things.cend());
}

bool ComponentInventory::contains(InventorySlot slot)
{
  return (m_things.count(slot) != 0);
}

InventorySlot ComponentInventory::operator[](EntityId entity)
{
  if (GAME.entities().exists(entity) == false) return InventorySlot::Invalid;

  auto iter = find(entity);

  if (iter != m_things.cend())
  {
    return iter->first;
  }

  return InventorySlot::Invalid;
}

EntityId ComponentInventory::operator[](InventorySlot slot)
{
  return (m_things.at(slot));
}

EntityId ComponentInventory::split(EntityId entity, unsigned int target_quantity)
{
  EntityId target_thing = EntityId::Mu();

  if (target_quantity > 0)
  {
    auto iter = find(entity);

    if (iter != m_things.cend())
    {
      EntityId source_thing = iter->second;
      unsigned int source_quantity = COMPONENTS.physical.valueOrDefault(source_thing).quantity();
      if (target_quantity < source_quantity)
      {
        target_thing = GAME.entities().clone(source_thing);
        COMPONENTS.physical[source_thing].quantity() = source_quantity - target_quantity;
        COMPONENTS.physical[target_thing].quantity() = target_quantity;
      }
    }
  }

  return target_thing;
}

EntityId ComponentInventory::remove(InventorySlot slot)
{
  EntityId removed_thing;
  if (m_things.count(slot) != 0)
  {
    removed_thing = m_things[slot];
    m_things.erase(slot);
  }
  return removed_thing;
}

EntityId ComponentInventory::remove(EntityId entity)
{
  EntityId removed_thing;

  auto iter = find(entity);
  if (iter != m_things.cend())
  {
    removed_thing = iter->second;
    m_things.erase(iter);
  }
  return removed_thing;
}

EntityId ComponentInventory::get_largest_thing()
{
  auto iter_largest = m_things.cbegin();

  for (EntityMap::const_iterator iter = m_things.cbegin();
       iter != m_things.cend(); ++iter)
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
    EntityId ref = thing_pair.second;
    return ((ref->isSubtypeOf("DynamicEntity")) && 
      (static_cast<int>(ref->getModifiedProperty("hp", 0)) > 0));
  });

  if (iter != m_things.cend())
  {
    return iter->second;
  }
  else
  {
    return EntityId::Mu();
  }
}

EntityMap::iterator ComponentInventory::find_if(std::function<bool(EntityPair const&)> functor)
{
  EntityMap::iterator iter =
    std::find_if(m_things.begin(), m_things.end(), functor);
  return iter;
}

EntityMap::iterator ComponentInventory::find(EntityId target_id)
{
  EntityMap::iterator iter =
    std::find_if(
      m_things.begin(),
      m_things.end(),
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

  return (COMPONENTS.physical[a].totalVolume() < COMPONENTS.physical[b].totalVolume());
}