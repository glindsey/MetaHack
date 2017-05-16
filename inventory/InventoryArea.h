#pragma once

#include "stdafx.h"

#include "GUIWindow.h"
#include "inventory/InventorySlot.h"
#include "Observer.h"

// Forward declarations
class Container;
class InventorySelection;
class Entity;
class EntityId;

class InventoryArea :
  public metagui::Window
{
public:
  explicit InventoryArea(std::string name, 
                         InventorySelection& inventory_selection,
                         sf::IntRect dimensions);
  virtual ~InventoryArea();

protected:
  virtual void drawContents_(sf::RenderTexture& texture, int frame) override;

  virtual bool onEvent_V(Event const& event) override;

private:
  /// Inventory selection we are bound to.
  InventorySelection& m_inventorySelection;
};