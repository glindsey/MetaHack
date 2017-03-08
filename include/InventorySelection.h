#pragma once

#include "stdafx.h"

#include "InventorySlot.h"
#include "Subject.h"

// Forward declarations
class Container;
class Entity;
class EntityId;

/// InventorySelection is sort of the controller that binds an InventoryView
/// to a Entity to be displayed.
class InventorySelection : public Subject
{
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

  EntityId get_entity(InventorySlot selection);

  /// @todo Not sure if these actually belong here, or somewhere else.
  static char get_character(InventorySlot slot);
  static char get_character(unsigned int slot_number);
  static InventorySlot get_slot(char character);

protected:

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};