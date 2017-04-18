#include "stdafx.h"

#include "inventory/Inventory.h"

#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "game/GameState.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
  //dtor
}

void from_json(json const& j, Inventory& obj)
{
  if (!j.is_object() || j.size() == 0)
  {
    obj = Inventory();
  }
  else
  {
    // *** add Component-specific assignments here ***
    //obj.m_member = j["member"];
  }
}

void to_json(json& j, Inventory const& obj)
{
  j = json::object();
  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}

bool Inventory::add(EntityId entity)
{
  // If entity is Mu, exit returning false.
  if (entity == EntityId::Mu())
  {
    return false;
  }

  // If the entity is the player, it goes into slot 0.
  if (entity == GAME.getPlayer())
  {
    if (things_.count(InventorySlot::Zero) != 0)
    {
      /// @todo Move anything in this slot to a new slot.  This will be required
      ///       if it's possible to change the ID of the player DynamicEntity.
      CLOG(ERROR, "Inventory") << "slot 0 of inventory already contains the player";
    }
    things_[InventorySlot::Zero] = entity;
    return true;
  }

  auto found_thing_id = find(entity);

  if (found_thing_id == things_.cend())
  {
    for (InventorySlot slot = InventorySlot::Min; slot < InventorySlot::Max; ++slot)
    {
      if (things_.count(slot) == 0)
      {
        things_[slot] = entity;
        consolidate_items();
        return true;
      }
    }
  }

  return false;
}

void Inventory::clear()
{
  things_.clear();
}

size_t Inventory::count()
{
  return things_.size();
}

EntityMap::iterator Inventory::begin()
{
  return std::begin(things_);
}

EntityMap::iterator Inventory::end()
{
  return std::end(things_);
}

EntityMap::const_iterator Inventory::cbegin()
{
  return things_.cbegin();
}

EntityMap::const_iterator Inventory::cend()
{
  return things_.cend();
}

void Inventory::consolidate_items()
{
  auto first_iter = std::begin(things_);
  while (first_iter != std::end(things_))
  {
    ++first_iter;
    auto second_iter = first_iter;
    --first_iter;

    while (second_iter != std::end(things_))
    {
      EntityId first_thing = first_iter->second;
      EntityId second_thing = second_iter->second;

      if (first_thing->can_merge_with(second_thing))
      {
        auto first_quantity = COMPONENTS.physical.value(first_thing).quantity();
        auto second_quantity = COMPONENTS.physical.value(second_thing).quantity();
        COMPONENTS.physical[first_thing].quantity() = first_quantity + second_quantity;
        COMPONENTS.physical[second_thing].quantity() = 0;

        auto second_iter_copy = second_iter;
        --second_iter;
        things_.erase(second_iter_copy);
      }
      ++second_iter;
    }
    ++first_iter;
  }
}

bool Inventory::contains(EntityId entity)
{
  if (GAME.entities().exists(entity) == false) return false;

  return (find(entity) != things_.cend());
}

bool Inventory::contains(InventorySlot slot)
{
  return (things_.count(slot) != 0);
}

InventorySlot Inventory::operator[](EntityId entity)
{
  if (GAME.entities().exists(entity) == false) return InventorySlot::Invalid;

  auto iter = find(entity);

  if (iter != things_.cend())
  {
    return iter->first;
  }

  return InventorySlot::Invalid;
}

EntityId Inventory::operator[](InventorySlot slot)
{
  return (things_.at(slot));
}

EntityId Inventory::split(EntityId entity, unsigned int target_quantity)
{
  EntityId target_thing = EntityId::Mu();

  if (target_quantity > 0)
  {
    auto iter = find(entity);

    if (iter != things_.cend())
    {
      EntityId source_thing = iter->second;
      unsigned int source_quantity = COMPONENTS.physical.value(source_thing).quantity();
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

EntityId Inventory::remove(InventorySlot slot)
{
  EntityId removed_thing;
  if (things_.count(slot) != 0)
  {
    removed_thing = things_[slot];
    things_.erase(slot);
  }
  return removed_thing;
}

EntityId Inventory::remove(EntityId entity)
{
  EntityId removed_thing;

  auto iter = find(entity);
  if (iter != things_.cend())
  {
    removed_thing = iter->second;
    things_.erase(iter);
  }
  return removed_thing;
}

EntityId Inventory::get_largest_thing()
{
  auto iter_largest = things_.cbegin();

  for (EntityMap::const_iterator iter = things_.cbegin();
       iter != things_.cend(); ++iter)
  {
    if (isSmallerThan(iter_largest->second, iter->second))
    {
      iter_largest = iter;
    }
  }
  return iter_largest->second;
}

EntityId Inventory::getEntity()
{
  auto iter =
    find_if([&](const EntityPair& thing_pair)
  {
    EntityId ref = thing_pair.second;
    return ((ref->isSubtypeOf("DynamicEntity")) && 
      (static_cast<int>(ref->getModifiedProperty("hp", 0)) > 0));
  });

  if (iter != things_.cend())
  {
    return iter->second;
  }
  else
  {
    return EntityId::Mu();
  }
}

EntityMap::iterator Inventory::find_if(std::function<bool(EntityPair const&)> functor)
{
  EntityMap::iterator iter =
    std::find_if(things_.begin(), things_.end(), functor);
  return iter;
}

EntityMap::iterator Inventory::find(EntityId target_id)
{
  EntityMap::iterator iter =
    std::find_if(
      things_.begin(),
      things_.end(),
      [&](EntityPair const& thing_pair)
  {
    return thing_pair.second == target_id;
  });

  return iter;
}

bool Inventory::isSmallerThan(EntityId a, EntityId b)
{
  if ((a == EntityId::Mu()) || (b == EntityId::Mu())) return false;
  if (!COMPONENTS.physical.exists(a) || !COMPONENTS.physical.exists(b)) return false;

  return (COMPONENTS.physical[a].totalVolume() < COMPONENTS.physical[b].totalVolume());
}