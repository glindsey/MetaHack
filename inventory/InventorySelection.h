#pragma once

#include "stdafx.h"

#include "entity/EntityId.h"
#include "inventory/InventorySlot.h"
#include "Subject.h"

// Forward declarations
class Container;
class Entity;

/// InventorySelection is sort of the controller that binds an InventoryView
/// to a Entity to be displayed.
class InventorySelection : public Subject
{
  struct EventEntityChanged : public ConcreteEvent<EventEntityChanged>
  {
    EventEntityChanged(EntityId id_)
      :
      entityId(id_)
    {}

    EntityId const entityId;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | id:" << entityId;
    }
  };

  struct EventSelectionChanged : public ConcreteEvent<EventSelectionChanged>
  {
    EventSelectionChanged(std::vector<InventorySlot> slots_, unsigned int quantity_)
      :
      slots{ slots_ },
      quantity{ quantity_ }
    {}

    std::vector<InventorySlot> const slots;
    unsigned int quantity;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | quantity: " << quantity << " | slots:";
      for (auto& slot : slots)
      {
        os << " " << slot;
      }
    }
  };

public:
  InventorySelection();
  explicit InventorySelection(EntityId entity);
  virtual ~InventorySelection();

  EntityId get_viewed() const;
  void set_viewed(EntityId entity);

  void toggle_selection(InventorySlot selection);
  size_t get_selected_slot_count() const;
  std::vector<InventorySlot> const& get_selected_slots();
  std::vector<EntityId> get_selected_things();
  void clear_selected_slots();

  unsigned int get_selected_quantity() const;
  unsigned int get_max_quantity() const;
  unsigned int reset_selected_quantity();
  bool set_selected_quantity(unsigned int amount);
  bool inc_selected_quantity();
  bool dec_selected_quantity();

  EntityId getEntity(InventorySlot selection);

  /// @todo Not sure if these actually belong here, or somewhere else.
  static char get_character(InventorySlot slot);
  static char get_character(unsigned int slot_number);
  static InventorySlot get_slot(char character);

protected:
  virtual std::unordered_set<EventID> registeredEvents() const override;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};