#ifndef INVENTORYAREA_H
#define INVENTORYAREA_H

#include "stdafx.h"

#include "GUIWindow.h"
#include "InventorySlot.h"

enum class InventoryType
{
  Inside,
  Around
};

// Forward declarations
class Container;
class Thing;
class ThingId;

class InventoryArea :
  public metagui::Window
{
public:
  explicit InventoryArea(sf::IntRect dimensions);
  virtual ~InventoryArea();

  ThingId get_viewed() const;
  void set_viewed(ThingId thing);

  void toggle_selection(InventorySlot selection);
  unsigned int get_selected_slot_count() const;
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

protected:
  virtual void render_contents_(sf::RenderTexture& texture, int frame) override;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // STATUSAREA_H
