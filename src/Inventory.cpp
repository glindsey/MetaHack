#include "stdafx.h"

#include "Inventory.h"

#include "GameState.h"
#include "Entity.h"
#include "EntityPool.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
  //dtor
}

bool Inventory::add(EntityId entity)
{
  // If entity is Mu, exit returning false.
  if (entity == EntityId::Mu())
  {
    return false;
  }

  // If the entity is the player, it goes into slot 0.
  if (entity == GAME.get_player())
  {
    if (things_.count(INVSLOT_ZERO) != 0)
    {
      /// @todo Move anything in this slot to a new slot.  This will be required
      ///       if it's possible to change the ID of the player DynamicEntity.
      MAJOR_ERROR("Slot 0 of inventory already contains the player!");
    }
    things_[INVSLOT_ZERO] = entity;
    return true;
  }

  auto found_thing_id = find(entity);

  if (found_thing_id == things_.cend())
  {
    for (InventorySlot slot = INVSLOT_MIN; slot < INVSLOT_MAX; ++slot)
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
        auto first_quantity = first_thing->get_quantity();
        auto second_quantity = second_thing->get_quantity();
        first_thing->set_quantity(first_quantity + second_quantity);

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
  if (GAME.get_entities().exists(entity) == false) return false;

  return (find(entity) != things_.cend());
}

bool Inventory::contains(InventorySlot slot)
{
  return (things_.count(slot) != 0);
}

InventorySlot Inventory::operator[](EntityId entity)
{
  if (GAME.get_entities().exists(entity) == false) return INVSLOT_INVALID;

  auto iter = find(entity);

  if (iter != things_.cend())
  {
    return iter->first;
  }

  return INVSLOT_INVALID;
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
      unsigned int source_quantity = source_thing->get_quantity();
      if (target_quantity < source_quantity)
      {
        target_thing = GAME.get_entities().clone(source_thing);
        source_thing->set_quantity(source_quantity - target_quantity);
        target_thing->set_quantity(target_quantity);
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
    if (is_smaller_than(iter_largest->second, iter->second))
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
    return ((ref->is_subtype_of("DynamicEntity")) && (ref->get_modified_property<int>("hp") > 0));
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

bool Inventory::is_smaller_than(EntityId a, EntityId b)
{
  if ((a == EntityId::Mu()) || (b == EntityId::Mu())) return false;

  return (a->get_mass() < b->get_mass());
}