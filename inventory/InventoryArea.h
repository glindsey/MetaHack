#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <memory>

#include "inventory/InventorySlot.h"
#include "views/EntityCollectionGUIView.h"

// Forward declarations
class Container;
class Entity;
class EntityId;
class GameState;
class InventorySelection;

class InventoryArea
  :
  public Object
{
public:
  explicit InventoryArea(sf::RenderWindow& renderWindow,
                         sfg::SFGUI& sfgui,
                         sfg::Desktop& desktop,
                         std::string name,
                         InventorySelection& inventory_selection,
                         GameState& gameState);
  virtual ~InventoryArea();

protected:
  virtual void drawContents_(sf::RenderTexture& texture, int frame);

  virtual bool onEvent(Event const& event);

private:
  /// Reference to the render window.
  sf::RenderWindow& m_renderWindow;

  /// Reference to the SFGUI API.
  sfg::SFGUI& m_sfgui;

  /// Reference to the SFGUI desktop.
  sfg::Desktop& m_desktop;

  /// The main SFGUI window.
  sfg::Window::Ptr m_window;

  /// The box layout widget for the window.
  sfg::Box::Ptr m_layout;

  /// The box containing config buttons.
  sfg::Box::Ptr m_buttonsBox;

  /// The scrolled window for viewing the inventory.
  sfg::ScrolledWindow::Ptr m_inventoryWindow;

  /// The EntityCollectionGUIView object that provides the layout widget that attaches
  /// to the scrolled window.
  std::unique_ptr<EntityCollectionGUIView> m_guiView;

  sfg::Table::Ptr m_inventoryLayout;

  /// @todo Support a list *or* a grid layout.

  GameState& m_gameState;

  /// Inventory selection we are bound to.
  InventorySelection& m_inventorySelection;
};
