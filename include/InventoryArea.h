#pragma once

#include "stdafx.h"

#include "GUIWindow.h"
#include "InventorySlot.h"
#include "Observer.h"

// Forward declarations
class Container;
class InventorySelection;
class Entity;
class EntityId;

class InventoryArea :
  public metagui::Window,
  public Observer
{
public:
  explicit InventoryArea(std::string name, 
                         InventorySelection& inventory_selection,
                         sf::IntRect dimensions);
  virtual ~InventoryArea();

  virtual void notifyOfEvent_(Observable& observed, Event event);

protected:
  virtual void render_contents_(sf::RenderTexture& texture, int frame) override;

  void draw_thing(EntityId thing,
                  sf::RenderTarget& target,
                  Vec2f target_coords,
                  unsigned int target_size = 0,
                  bool use_lighting = true,
                  int frame = 0);

private:
  /// Inventory selection we are bound to.
  InventorySelection& m_inventory_selection;
};