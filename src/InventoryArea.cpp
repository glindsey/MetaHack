#include "stdafx.h"

#include "InventoryArea.h"

#include "App.h"
#include "EntityView.h"
#include "IConfigSettings.h"
#include "IGraphicViews.h"
#include "Inventory.h"
#include "InventorySelection.h"
#include "MapTile.h"
#include "Service.h"
#include "Entity.h"
#include "EntityPool.h"
#include "TileSheet.h"

InventoryArea::InventoryArea(std::string name,
                             InventorySelection& inventory_selection,
                             sf::IntRect dimensions)
  : 
  metagui::Window(name, dimensions),
  m_inventory_selection(inventory_selection)
{
  SET_UP_LOGGER("InventoryArea", true);
  m_inventory_selection.addObserver(*this, EventID::All);
}

InventoryArea::~InventoryArea()
{
  m_inventory_selection.removeObserver(*this);
}

void InventoryArea::drawContents_(sf::RenderTexture& texture, int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto& views = Service<IGraphicViews>::get();
  auto& entity_pool = GAME.get_entities();

  // Dimensions of the pane.
  sf::IntRect pane_dims = getRelativeDimensions();

  float line_spacing_y = the_default_font.getLineSpacing(config.get("text_default_size").as<Integer>());
  float item_spacing_y = 4.0f;

  // Text offsets relative to the background rectangle.
  float text_offset_x = config.get("window_text_offset_x").as<Real>();
  float text_offset_y = config.get("window_text_offset_y").as<Real>();

  // Get a reference to the location we're referring to.
  auto& viewed_thing = m_inventory_selection.get_viewed();
  if (viewed_thing == EntityId::Mu())
  {
    setText("Invalid Viewed Object!");
    return;
  }

  // Start at the top and work down.
  float text_coord_x = text_offset_x;
  float text_coord_y = text_offset_y + (line_spacing_y * 1.5f);

  Inventory& inventory = viewed_thing->get_inventory();
  auto& selected_slots = m_inventory_selection.get_selected_slots();

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
    sf::Color fg_color = config.get("text_color").as<Color>();
    size_t selection_order = 0;
    auto slot_iter = std::find(selected_slots.begin(),
                               selected_slots.end(),
                               slot);

    if (slot_iter != selected_slots.end())
    {
      fg_color = config.get("text_highlight_color").as<Color>();
      selection_order = (slot_iter - selected_slots.begin()) + 1;
    }

    // 2. Display the selection order.
    if (selection_order != 0)
    {
      std::stringstream selection_number;
      selection_number << "[" << selection_order << "]" << std::endl;
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text_mono_default_size").as<Integer>());
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

      item_char = InventorySelection::get_character(slot_number);

      slot_id << item_char << ":";
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text_mono_default_size").as<Integer>());
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

    unsigned int wield_location;
    WearLocation wear_location;
    bool wielding = viewed_thing->is_wielding(entity, wield_location);
    bool wearing = viewed_thing->has_equipped(entity, wear_location);

    // 5. TODO: Display "worn" or "equipped" icon if necessary.
    if (wielding)
    {
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text_mono_default_size").as<Integer>());
      render_text.setString("W");
      render_text.setPosition(text_coord_x + 11, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }
    else if (wearing)
    {
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(config.get("text_mono_default_size").as<Integer>());
      render_text.setString("E");
      render_text.setPosition(text_coord_x + 11, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }

    // 6. Display the item name.
    std::stringstream item_name;
    if (selection_order == 1)
    {
      unsigned int max_quantity = entity->get_quantity();
      unsigned int selected_quantity = m_inventory_selection.get_selected_quantity();
      if ((max_quantity > 1) && (selected_quantity < max_quantity))
      {
        item_name << "(Sel: " << selected_quantity << ") ";
      }
    }

    item_name << entity->get_identifying_string(ArticleChoice::Indefinite, UsePossessives::No);

    if (wielding)
    {
      item_name << " (" << viewed_thing->get_bodypart_description(BodyPart::Hand, wield_location) << ")";
    }
    else if (wearing)
    {
      item_name << " (" << viewed_thing->get_bodypart_description(wear_location.part, wear_location.number) << ")";
    }

    render_text.setFont(the_default_font);
    render_text.setCharacterSize(config.get("text_default_size").as<Integer>());
    render_text.setString(item_name.str());
    render_text.setPosition(text_coord_x + 80 + line_spacing_y,
                            text_coord_y + 1);
    render_text.setColor(fg_color);
    texture.draw(render_text);

    if (text_coord_y > pane_dims.height) break;
    text_coord_y += line_spacing_y + item_spacing_y;

    // 7. Display a nice separator line.
    sf::RectangleShape separator_line;
    separator_line.setPosition(text_coord_x + 10, text_coord_y);
    separator_line.setSize(RealVec2(static_cast<float>(pane_dims.width - 25), 1.0f));
    separator_line.setFillColor(config.get("window_border_color").as<Color>());
    texture.draw(separator_line);

    text_coord_y += item_spacing_y;
  }

  // Figure out the title.
  // TODO: Might want to define a specific "get_inventory_name()" method
  //       for Entity that defaults to "XXXX's inventory" but can be
  //       overridden to say stuff like "Entities on the floor".
  sf::String title_string = viewed_thing->get_possessive_of("inventory");
  title_string[0] = toupper(title_string[0]);
  setText(title_string);
  return;
}

void InventoryArea::onEvent(Event const & event)
{
  /// @todo Flesh this out a bit more.
  ///       Right now we just set the "dirty" flag for the view so it is redrawn.
  flagForRedraw();
}
