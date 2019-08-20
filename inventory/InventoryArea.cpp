#include "inventory/InventoryArea.h"

#include <cctype>

#include "components/ComponentInventory.h"
#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "config/Settings.h"
#include "game/App.h"
#include "game/GameState.h"
#include "entity/EntityFactory.h"
#include "inventory/InventorySelection.h"
#include "map/Map.h"
#include "maptile/MapTile.h"
#include "systems/Manager.h"
#include "systems/SystemChoreographer.h"
#include "systems/SystemGeometry.h"
#include "systems/SystemNarrator.h"
#include "types/common.h"
#include "views/EntityView.h"
#include "views/EntityCollectionGUIListView.h"


InventoryArea::InventoryArea(sf::RenderWindow& renderWindow,
                             sfg::SFGUI& sfgui,
                             sfg::Desktop& desktop,
                             std::string name,
                             InventorySelection& inventory_selection,
                             GameState& gameState) :
  Object({}),
  m_renderWindow{ renderWindow },
  m_sfgui{ sfgui },
  m_desktop{ desktop },
  m_gameState{ gameState },
  m_inventorySelection{ inventory_selection }
{
  m_window = sfg::Window::Create();
  m_window->SetTitle("Inventory of (whatever)");

  // ==========================================================================
  // The window for showing the inventory is going to have the following
  // hierarchy:
  //
  // - Window
  //   - m_layout: Box (VERTICAL)
  //     - m_buttonsBox: Box (HORIZONTAL)
  //       - (Various config buttons will go here eventually, blank for now)
  //     - m_inventoryWindow: ScrolledWindow
  //       - m_guiView.getWidget(): Table
  //         - In list view:
  //           - Each row is an item, with a header row at the top
  //           - Column 0 is an icon representing the item
  //           - Column 1 is the item letter
  //           - Column 2 is the item description
  //         - In grid view:
  //           - Each cell is an item containing an icon representing the item
  //           - Possibly also shows the item letter in the corner
  //           - Hovering over the item gives you the description

  m_layout = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
  m_buttonsBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
  m_inventoryWindow = sfg::ScrolledWindow::Create();

  /// @note Not sure if this be created by default or not.
  /// And in any case, m_inventorySelection will be going away.
  m_guiView.reset(
    NEW EntityCollectionGUIListView(
      m_sfgui,
      m_gameState.components().inventory[m_inventorySelection.getViewed()].getCollection()
    )
  );

  // Set the scrolled window's scrollbar policies.
  m_inventoryWindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC |
                                        sfg::ScrolledWindow::VERTICAL_AUTOMATIC);

  // Add the layout box to the scrolled window using a viewport.
  m_inventoryWindow->AddWithViewport(m_guiView->getWidget());

  // Set the scrolled window's minimum size; for X this should be the width of
  // the containing box, minus 2 * spacing.
  m_inventoryWindow->SetRequisition({ 80.f, 40.f });

  // Set the input box's minimum size; for X this should be the width of the
  // containing box, minus 2 * spacing.
  m_inventoryWindow->SetRequisition({ 80.f, 20.f });

  /// @todo Add child items to the inventory layout (for the various inventory items).

  // Pack the child items into the overall layout.
  m_layout->Pack(m_buttonsBox, false, true);
  m_layout->Pack(m_inventoryWindow, true, true);

  // Add the box to the window, and the window to the desktop.
  m_window->Add(m_layout);
  m_desktop.Add(m_window);

  // Set initial window size. (Has to be done after a widget is added to a hierarchy.)
  auto deskSize = m_renderWindow.getSize();
  m_window->SetAllocation(sf::FloatRect(deskSize.x * 0.7f, 0.0f, deskSize.x * 0.3f, deskSize.y));

  // Start observing the Choreographer for "player changed" events, and the Geometry system
  // for "entity moved" / "entity changed maps" events.
  subscribeTo(SYSTEMS.choreographer(), Systems::Choreographer::EventPlayerChanged::id);
  subscribeTo(SYSTEMS.geometry(), Systems::Geometry::EventEntityMoved::id);
  subscribeTo(SYSTEMS.geometry(), Systems::Geometry::EventEntityChangedMaps::id);

  // Add an observer to the inventory selection model.
  subscribeTo(m_inventorySelection, EventID::All);

  /// TEMPORARY: Hide the SFGUI window for the time being, while we work on other stuff.
  //m_window->Show(false);
}

InventoryArea::~InventoryArea()
{
  unsubscribeFrom(m_inventorySelection);
}

void InventoryArea::drawContents_(sf::RenderTexture& texture, int frame)
{
  // auto& config = Config::settings();
  // auto& entity_pool = m_gameState.entities();

  // // Dimensions of the pane.
  // sf::IntRect pane_dims = getRelativeDimensions();

  // float line_spacing_y = the_default_font.getLineSpacing(config.get("text-default-size"));
  // float item_spacing_y = 4.0f;

  // // Text offsets relative to the background rectangle.
  // RealVec2 text_offset = config.get("window-text-offset");

  // // Get a reference to the location we're referring to.
  // auto viewed_thing = m_inventorySelection.getViewed();
  // if (viewed_thing == EntityId::Void)
  // {
  //   setText("Invalid Viewed Object!");
  //   return;
  // }

  // // Start at the top and work down.
  // float text_coord_x = text_offset.x;
  // float text_coord_y = text_offset.y + (line_spacing_y * 1.5f);

  // auto& inventory = COMPONENTS.inventory[viewed_thing];
  // auto& selected_slots = m_inventorySelection.getSelectedSlots();

  // /// @todo At the moment this does not split lines that are too long, instead
  // ///       truncating them at the edge of the box.  This must be fixed.
  // /// @todo Also need to display some details about the item, such as whether it
  // ///       is equipped, what magicules are equipped to it, et cetera.
  // for (auto iter = inventory.cbegin();
  //      iter != inventory.cend();
  //      ++iter)
  // {
  //   auto& slot = (*iter).first;
  //   EntityId entity = (*iter).second;
  //   unsigned int slot_number = static_cast<unsigned int>(slot);
  //   sf::Text render_text;

  //   // 1. Figure out whether this is selected or not, and set FG color.
  //   Color fg_color = config.get("text-color");
  //   size_t selectionOrder = 0;
  //   auto slot_iter = std::find(selected_slots.begin(),
  //                              selected_slots.end(),
  //                              slot);

  //   if (slot_iter != selected_slots.end())
  //   {
  //     fg_color = config.get("text-highlight-color");
  //     selectionOrder = (slot_iter - selected_slots.begin()) + 1;
  //   }

  //   // 2. Display the selection order.
  //   if (selectionOrder != 0)
  //   {
  //     std::stringstream selection_number;
  //     selection_number << "[" << selectionOrder << "]" << std::endl;
  //     render_text.setFont(the_default_mono_font);
  //     render_text.setCharacterSize(config.get("text-mono-default-size"));
  //     render_text.setString(selection_number.str());
  //     render_text.setPosition(text_coord_x + 26, text_coord_y);
  //     render_text.setFillColor(fg_color);
  //     texture.draw(render_text);
  //   }

  //   // 3. Display the slot ID.
  //   if (slot_number < 27)
  //   {
  //     std::stringstream slot_id;
  //     char item_char;

  //     item_char = InventorySelection::getCharacter(slot_number);

  //     slot_id << item_char << ":";
  //     render_text.setFont(the_default_mono_font);
  //     render_text.setCharacterSize(config.get("text-mono-default-size"));
  //     render_text.setString(slot_id.str());
  //     render_text.setPosition(text_coord_x + 55, text_coord_y);
  //     render_text.setFillColor(fg_color);
  //     texture.draw(render_text);
  //   }

  //   // 4. Display the tile representing the item.
  //   auto entity_view = std::unique_ptr<EntityView>(NEW EntityView2D(entity));
  //   entity_view->setLocation({ static_cast<float>(text_coord_x + 75), static_cast<float>(text_coord_y) });
  //   entity_view->setSize({ line_spacing_y - 1, line_spacing_y - 1 } );
  //   entity_view->draw(texture,
  //                     &(SYSTEMS.lighting()),
  //                     true,
  //                     frame);

  //   BodyLocation wieldLocation;
  //   BodyLocation wearLocation;
  //   bool wielding = COMPONENTS.bodyparts.existsFor(entity) && (COMPONENTS.bodyparts[entity].getWieldedEntity(wieldLocation) != EntityId::Void);
  //   bool wearing = COMPONENTS.bodyparts.existsFor(entity) && (COMPONENTS.bodyparts[entity].getWornEntity(wearLocation) != EntityId::Void);

  //   // 5. TODO: Display "worn" or "equipped" icon if necessary.
  //   if (wielding)
  //   {
  //     render_text.setFont(the_default_mono_font);
  //     render_text.setCharacterSize(config.get("text-mono-default-size"));
  //     render_text.setString("W");
  //     render_text.setPosition(text_coord_x + 11, text_coord_y);
  //     render_text.setFillColor(fg_color);
  //     texture.draw(render_text);
  //   }
  //   else if (wearing)
  //   {
  //     render_text.setFont(the_default_mono_font);
  //     render_text.setCharacterSize(config.get("text-mono-default-size"));
  //     render_text.setString("E");
  //     render_text.setPosition(text_coord_x + 11, text_coord_y);
  //     render_text.setFillColor(fg_color);
  //     texture.draw(render_text);
  //   }

  //   // 6. Display the item name.
  //   std::stringstream item_name;
  //   if (selectionOrder == 1)
  //   {
  //     unsigned int maxQuantity = COMPONENTS.quantity.valueOr(entity, 1);
  //     unsigned int selectedQuantity = m_inventorySelection.getSelectedQuantity();
  //     if ((maxQuantity > 1) && (selectedQuantity < maxQuantity))
  //     {
  //       item_name << "(Sel: " << selectedQuantity << ") ";
  //     }
  //   }

  //   item_name << SYSTEMS.narrator().getDescriptiveString(entity, ArticleChoice::Indefinite, UsePossessives::No);

  //   if (wielding)
  //   {
  //     item_name << " (" << SYSTEMS.narrator().getBodypartDescription(viewed_thing, wieldLocation) << ")";
  //   }
  //   else if (wearing)
  //   {
  //     item_name << " (" << SYSTEMS.narrator().getBodypartDescription(viewed_thing, wearLocation) << ")";
  //   }

  //   render_text.setFont(the_default_font);
  //   render_text.setCharacterSize(config.get("text-default-size"));
  //   render_text.setString(item_name.str());
  //   render_text.setPosition(text_coord_x + 80 + line_spacing_y,
  //                           text_coord_y + 1);
  //   render_text.setFillColor(fg_color);
  //   texture.draw(render_text);

  //   if (text_coord_y > pane_dims.height) break;
  //   text_coord_y += line_spacing_y + item_spacing_y;

  //   // 7. Display a nice separator line.
  //   sf::RectangleShape separator_line;
  //   Color border_color = config.get("window-border-color");
  //   separator_line.setPosition(text_coord_x + 10, text_coord_y);
  //   separator_line.setSize(RealVec2(static_cast<float>(pane_dims.width - 25), 1.0f));
  //   separator_line.setFillColor(border_color);
  //   texture.draw(separator_line);

  //   text_coord_y += item_spacing_y;
  // }

  // // Figure out the title.
  // // TODO: Might want to define a specific "get_inventory_name()" method
  // //       for Entity that defaults to "XXXX's inventory" but can be
  // //       overridden to say stuff like "Entities on the floor".
  // sf::String title_string = SYSTEMS.narrator().getPossessiveString(viewed_thing, "inventory");
  // title_string[0] = toupper(title_string[0]);
  // setText(title_string);
  return;
}

bool InventoryArea::onEvent(Event const& event)
{
  EntityId player = SYSTEMS.choreographer().player();
  EntityId location = EntityId::Void;

  auto id = event.getId();
  bool recreateInventoryView = false;

  if (id == Systems::Choreographer::EventPlayerChanged::id)
  {
    recreateInventoryView = true;
  }
  else if (id == Systems::Geometry::EventEntityMoved::id)
  {
    auto info = static_cast<Systems::Geometry::EventEntityMoved const&>(event);
    if (info.entity == player)
    {
      recreateInventoryView = true;
    }
  }
  else if (id == Systems::Geometry::EventEntityChangedMaps::id)
  {
    auto info = static_cast<Systems::Geometry::EventEntityChangedMaps const&>(event);
    if (info.entity == player)
    {
      recreateInventoryView = true;
    }
  }
  else
  {
    return Object::onEvent(event);
  }

  /// @todo We'll also have to handle when the viewed collection (i.e. the inventory) changes.

  if (recreateInventoryView == true)
  {
    CLOG(TRACE, "InventoryArea") << "Got event " << event;

    auto& components = m_gameState.components();

    if (components.position.existsFor(player))
    {
      location = components.position[player].location();
      CLOG(TRACE, "InventoryArea") << "New player location is " << location;
    }

    sf::String title_string = SYSTEMS.narrator().getPossessiveString(location, "inventory");
    title_string[0] = toupper(title_string[0]);
    m_window->SetTitle(title_string);

    m_inventoryWindow->Remove(m_guiView->getWidget());
    m_guiView.reset(
      NEW EntityCollectionGUIListView(
        m_sfgui,
        m_gameState.components().inventory[location].getCollection()
      )
    );
    m_inventoryWindow->AddWithViewport(m_guiView->getWidget());

  }

  /// @todo Flesh this out a bit more.
  ///       Right now we just set the "dirty" flag for the view so it is redrawn.
  //flagForRedraw();

  return false;
}
