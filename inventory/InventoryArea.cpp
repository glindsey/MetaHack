#include "stdafx.h"

#include "inventory/InventoryArea.h"

#include "components/ComponentInventory.h"
#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "entity/EntityView.h"
#include "game/App.h"
#include "game/GameState.h"
#include "services/IConfigSettings.h"
#include "services/IGraphicViews.h"
#include "inventory/InventorySelection.h"
#include "maptile/MapTile.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"

InventoryArea::InventoryArea(std::string name,
                             InventorySelection& inventory_selection,
                             sf::IntRect dimensions)
  : 
  metagui::Window(name, dimensions),
  m_inventorySelection(inventory_selection)
{
  m_inventorySelection.addObserver(*this, EventID::All);
}

InventoryArea::~InventoryArea()
{
  m_inventorySelection.removeObserver(*this);
}

void InventoryArea::drawContents_(sf::RenderTexture& texture, int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto& views = Service<IGraphicViews>::get();
  auto& entity_pool = GAME.entities();

  // Dimensions of the pane.
  sf::IntRect pane_dims = getRelativeDimensions();

  float line_spacing_y = the_default_font.getLineSpacing(config.get("text-default-size"));
  float item_spacing_y = 4.0f;

  // Text offsets relative to the background rectangle.
  RealVec2 text_offset = config.get("window-text-offset");

  // Get a reference to the location we're referring to.
  auto& viewed_thing = m_inventorySelection.getViewed();
  if (viewed_thing == EntityId::Mu())
  {
    setText("Invalid Viewed Object!");
    return;
  }

  // Start at the top and work down.
  float text_coord_x = text_offset.x;
  float text_coord_y = text_offset.y + (line_spacing_y * 1.5f);

  ComponentInventory& inventory = COMPONENTS.inventory[viewed_thing];
  auto& selected_slots = m_inventorySelection.getSelectedSlots();

  /// @todo At the moment this does not split lines that are too long, instead
  ///       truncating them at the edge of the box.  This must be fixed.
  /// @todo Also need to display some details about the item, such as whether it
  ///       is equipped, what magicules are equipped to it, et cetera.
  for (auto iter = inventory.cbegin();
       iter != inventory.cend();
       ++iter)
  {
    auto& slot = (*iter).first;
    EntityId entity = (*iter).second;
    unsigned int slot_number = static_cast<unsigned int>(slot);
    sf::Text render_text;

    // 1. Figure out whether this is selected or not, and set FG color.
    Color fg_color = config.get("text-color");
    size_t selection_order = 0;
    auto slot_iter = std::find(selected_slots.begin(),
                               selected_slots.end(),
                               slot);

    if (slot_iter != selected_slots.end())
    {
      fg_color = config.get("text-highlight-color");
      selection_order = (slot_iter - selected_slots.begin()) + 1;
    }

    // 2. Display the selection order.
    if (selection_order != 0)
    {
      std::stringstream selection_number;
      selection_number << "[" << selection_order << "]" << std::endl;
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text-mono-default-size"));
      render_text.setString(selection_number.str());
      render_text.setPosition(text_coord_x + 26, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }

    // 3. Display the slot ID.
    if (slot_number < 27)
    {
      std::stringstream slot_id;
      char item_char;

      item_char = InventorySelection::getCharacter(slot_number);

      slot_id << item_char << ":";
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text-mono-default-size"));
      render_text.setString(slot_id.str());
      render_text.setPosition(text_coord_x + 55, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }

    // 4. Display the tile representing the item.
    auto entity_view = std::unique_ptr<EntityView>(views.createEntityView(entity_pool.get(entity)));
    entity_view->setLocation({ static_cast<float>(text_coord_x + 75), static_cast<float>(text_coord_y) });
    entity_view->setSize({ line_spacing_y - 1, line_spacing_y - 1 } );
    entity_view->draw(texture, false, true, frame);

    BodyLocation wieldLocation;
    BodyLocation wearLocation;
    bool wielding = COMPONENTS.bodyparts.existsFor(entity) && (COMPONENTS.bodyparts[entity].getWieldedEntity(wieldLocation) != EntityId::Mu());
    bool wearing = COMPONENTS.bodyparts.existsFor(entity) && (COMPONENTS.bodyparts[entity].getWornEntity(wearLocation) != EntityId::Mu());

    // 5. TODO: Display "worn" or "equipped" icon if necessary.
    if (wielding)
    {
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text-mono-default-size"));
      render_text.setString("W");
      render_text.setPosition(text_coord_x + 11, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }
    else if (wearing)
    {
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text-mono-default-size"));
      render_text.setString("E");
      render_text.setPosition(text_coord_x + 11, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }

    // 6. Display the item name.
    std::stringstream item_name;
    if (selection_order == 1)
    {
      unsigned int max_quantity = COMPONENTS.physical.valueOrDefault(entity).quantity();
      unsigned int selected_quantity = m_inventorySelection.getSelectedQuantity();
      if ((max_quantity > 1) && (selected_quantity < max_quantity))
      {
        item_name << "(Sel: " << selected_quantity << ") ";
      }
    }

    item_name << entity->getDescriptiveString(ArticleChoice::Indefinite, UsePossessives::No);

    if (wielding)
    {
      item_name << " (" << viewed_thing->getBodypartDescription(wieldLocation) << ")";
    }
    else if (wearing)
    {
      item_name << " (" << viewed_thing->getBodypartDescription(wearLocation) << ")";
    }

    render_text.setFont(the_default_font);
    render_text.setCharacterSize(config.get("text-default-size"));
    render_text.setString(item_name.str());
    render_text.setPosition(text_coord_x + 80 + line_spacing_y,
                            text_coord_y + 1);
    render_text.setColor(fg_color);
    texture.draw(render_text);

    if (text_coord_y > pane_dims.height) break;
    text_coord_y += line_spacing_y + item_spacing_y;

    // 7. Display a nice separator line.
    sf::RectangleShape separator_line;
    Color border_color = config.get("window-border-color");
    separator_line.setPosition(text_coord_x + 10, text_coord_y);
    separator_line.setSize(RealVec2(static_cast<float>(pane_dims.width - 25), 1.0f));
    separator_line.setFillColor(border_color);
    texture.draw(separator_line);

    text_coord_y += item_spacing_y;
  }

  // Figure out the title.
  // TODO: Might want to define a specific "get_inventory_name()" method
  //       for Entity that defaults to "XXXX's inventory" but can be
  //       overridden to say stuff like "Entities on the floor".
  sf::String title_string = viewed_thing->getPossessiveString("inventory");
  title_string[0] = toupper(title_string[0]);
  setText(title_string);
  return;
}

bool InventoryArea::onEvent_NVI_PreChildren(Event const & event)
{
  /// @todo Flesh this out a bit more.
  ///       Right now we just set the "dirty" flag for the view so it is redrawn.
  flagForRedraw();
  return true;
}
