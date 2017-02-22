#pragma once

#include "stdafx.h"

#include "InventorySlot.h"
#include "Observable.h"

enum class InventoryType
{
  Inside,
  Around
};

// Forward declarations
class Container;
class Thing;
class ThingId;

/// InventorySelection is sort of the controller that binds an InventoryView
/// to a Thing to be displayed.
class InventorySelection : public Observable
{
public:
  InventorySelection();
  explicit InventorySelection(ThingId thing);
  virtual ~InventorySelection();

  ThingId get_viewed() const;
  void set_viewed(ThingId thing);

  void toggle_selection(InventorySlot selection);
  size_t get_selected_slot_count() const;
  std::vector<InventorySlot> const& get_selected_slots();
  std::vector<ThingId> get_selected_things();
  void clear_selected_slots();

  unsigned int get_selected_quantity() const;
  unsigned int get_max_quantity() const;
  unsigned int reset_selected_quantity();
  bool set_selected_quantity(unsigned int amount);
  bool inc_selected_quantity();
  bool dec_selected_quantity();

  ThingId get_thing(InventorySlot selection);

  InventoryType get_inventory_type();
  void set_inventory_type(InventoryType type);

  /// @todo Not sure if these actually belong here, or somewhere else.
  static char get_character(InventorySlot slot);
  static char get_character(unsigned int slot_number);
  static InventorySlot get_slot(char character);

protected:

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};