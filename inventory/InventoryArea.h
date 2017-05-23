#pragma once

#include "stdafx.h"

#include "GUIWindow.h"
#include "inventory/InventorySlot.h"

// Forward declarations
class Container;
class Entity;
class EntityId;
class GameState;
class InventorySelection;

class InventoryArea :
  public metagui::Window
{
public:
  explicit InventoryArea(std::string name, 
                         InventorySelection& inventory_selection,
                         sf::IntRect dimensions,
                         GameState& gameState);
  virtual ~InventoryArea();

protected:
  virtual void drawContents_(sf::RenderTexture& texture, int frame) override;

  virtual bool onEvent_V(Event const& event) override;

private:
  GameState& m_gameState;

  /// Inventory selection we are bound to.
  InventorySelection& m_inventorySelection;
};