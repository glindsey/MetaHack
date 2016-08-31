#include "stdafx.h"

#include "InventoryArea.h"

#include "App.h"
#include "ConfigSettings.h"
#include "Inventory.h"
#include "MapTile.h"
#include "Thing.h"
#include "ThingManager.h"
#include "TileSheet.h"

struct InventoryArea::Impl
{
  /// Thing whose contents (or surroundings) are currently being viewed.
  ThingId viewed;

  /// Vector of selected inventory slots.
  std::vector< InventorySlot > selected_slots;

  /// Reference to quantity of topmost selected item.
  unsigned int selected_quantity;

  char get_character(InventorySlot slot)
  {
    return get_character(static_cast<unsigned int>(slot));
  }

  char get_character(unsigned int slot_number)
  {
    char character;

    if (slot_number == 0)
    {
      character = '@';
    }
    else if (slot_number < 26)
    {
      character = '`' + static_cast<char>(slot_number);
    }
    else if (slot_number < 52)
    {
      character = '@' + static_cast<char>(slot_number - 26);
    }
    else
    {
      character = ' ';
    }

    return character;
  }

  InventorySlot get_slot(char character)
  {
    unsigned int slot_number;
    unsigned int char_number = static_cast<unsigned int>(character);

    ASSERT_CONDITION(((char_number >= 0x40) && (char_number <= 0x5a)) ||
      ((char_number >= 0x61) && (char_number <= 0x7a)));

    if (char_number == 0x40)
    {
      slot_number = 0;
    }
    else if (char_number <= 0x5a)
    {
      slot_number = char_number - 64;
    }
    else
    {
      slot_number = char_number - 70;
    }

    return static_cast<InventorySlot>(slot_number);
  }
};

InventoryArea::InventoryArea(sf::IntRect dimensions)
  : metagui::Window("InventoryArea", dimensions),
  pImpl(NEW Impl())
{
  SET_UP_LOGGER("InventoryArea", true);
}

InventoryArea::~InventoryArea()
{
  //dtor
}

ThingId InventoryArea::get_viewed() const
{
  return pImpl->viewed;
}

void InventoryArea::set_viewed(ThingId thing)
{
  pImpl->viewed = thing;
  pImpl->selected_slots.clear();
}

void InventoryArea::toggle_selection(InventorySlot selection)
{
  if (pImpl->viewed == ThingId::Mu())
  {
    return;
  }

  Inventory& inventory = pImpl->viewed->get_inventory();

  if (inventory.contains(selection))
  {
    auto iter = std::find(std::begin(pImpl->selected_slots),
                          std::end(pImpl->selected_slots),
                          selection);
    if (iter == std::end(pImpl->selected_slots))
    {
      CLOG(TRACE, "InventoryArea") <<
        "Adding slot " << static_cast<unsigned int>(selection) <<
        "to selected things";
      pImpl->selected_slots.push_back(selection);
    }
    else
    {
      CLOG(TRACE, "InventoryArea") <<
        "Removing slot " << static_cast<unsigned int>(selection) <<
        "from selected things";
      pImpl->selected_slots.erase(iter);
    }

    reset_selected_quantity();
  }
}

unsigned int InventoryArea::get_selected_slot_count() const
{
  return pImpl->selected_slots.size();
}

std::vector<InventorySlot> const& InventoryArea::get_selected_slots()
{
  return pImpl->selected_slots;
}

std::vector<ThingId> InventoryArea::get_selected_things()
{
  std::vector<ThingId> things;

  if (pImpl->viewed != ThingId::Mu())
  {
    Inventory& inventory = pImpl->viewed->get_inventory();

    for (auto iter = std::begin(pImpl->selected_slots);
         iter != std::end(pImpl->selected_slots);
         ++iter)
    {
      ThingId thing = inventory[*iter];
      things.push_back(thing);
    }
  }

  return things;
}

void InventoryArea::clear_selected_slots()
{
  pImpl->selected_slots.clear();
}

unsigned int InventoryArea::get_selected_quantity() const
{
  return pImpl->selected_quantity;
}

unsigned int InventoryArea::get_max_quantity() const
{
  unsigned int result;

  if (pImpl->viewed == ThingId::Mu())
  {
    return 0;
  }

  Inventory& inventory = pImpl->viewed->get_inventory();

  if (pImpl->selected_slots.size() == 0)
  {
    result = 0;
  }
  else
  {
    ThingId thing = inventory[pImpl->selected_slots[0]];

    if (thing == ThingId::Mu())
    {
      result = 0;
    }
    else
    {
      result = thing->get_quantity();
    }
  }

  return result;
}

unsigned int InventoryArea::reset_selected_quantity()
{
  pImpl->selected_quantity = get_max_quantity();
  return pImpl->selected_quantity;
}

bool InventoryArea::set_selected_quantity(unsigned int amount)
{
  if (amount > 0)
  {
    unsigned int maximum = get_max_quantity();
    if (amount <= maximum)
    {
      pImpl->selected_quantity = amount;
      return true;
    }
  }
  return false;
}

bool InventoryArea::inc_selected_quantity()
{
  unsigned int maximum = get_max_quantity();
  if (pImpl->selected_quantity < maximum)
  {
    ++(pImpl->selected_quantity);
    return true;
  }
  return false;
}

bool InventoryArea::dec_selected_quantity()
{
  if (pImpl->selected_quantity > 1)
  {
    --(pImpl->selected_quantity);
    return true;
  }
  return false;
}

ThingId InventoryArea::get_thing(InventorySlot selection)
{
  ThingId viewed = pImpl->viewed;

  if (viewed == ThingId::Mu())
  {
    return ThingId::Mu();
  }

  Inventory& inventory = pImpl->viewed->get_inventory();

  if (inventory.contains(selection))
  {
    return inventory[selection];
  }
  else
  {
    CLOG(WARNING, "InventoryArea") <<
      "Requested non-existent inventory slot " <<
      static_cast<unsigned int>(selection) <<
      ", returning Mu!";
    return ThingId::Mu();
  }
}

void InventoryArea::render_contents_(sf::RenderTexture& texture, int frame)
{
  // Dimensions of the pane.
  sf::IntRect pane_dims = get_relative_dimensions();

  float line_spacing_y = the_default_font.getLineSpacing(the_config.get<unsigned int>("text_default_size"));
  float item_spacing_y = 4.0f;

  // Text offsets relative to the background rectangle.
  float text_offset_x = the_config.get<float>("window_text_offset_x");
  float text_offset_y = the_config.get<float>("window_text_offset_y");

  // Get a reference to the location we're referring to.
  if (pImpl->viewed == ThingId::Mu())
  {
    set_text(L"Invalid Viewed Object!");
    return;
  }

  // Start at the top and work down.
  float text_coord_x = text_offset_x;
  float text_coord_y = text_offset_y + (line_spacing_y * 1.5f);

  Inventory& inventory = pImpl->viewed->get_inventory();

  /// @todo At the moment this does not split lines that are too long, instead
  ///       truncating them at the edge of the box.  This must be fixed.
  /// @todo Also need to display some details about the item, such as whether it
  ///       is equipped, what magicules are equipped to it, et cetera.
  for (auto iter = inventory.cbegin();
       iter != inventory.cend();
       ++iter)
  {
    auto& slot = (*iter).first;
    ThingId thing = (*iter).second;
    unsigned int slot_number = static_cast<unsigned int>(slot);
    sf::Text render_text;

    // 1. Figure out whether this is selected or not, and set FG color.
    sf::Color fg_color = the_config.get<sf::Color>("text_color");
    unsigned int selection_order = 0;
    auto slot_iter = std::find(pImpl->selected_slots.begin(),
                               pImpl->selected_slots.end(),
                               slot);

    if (slot_iter != pImpl->selected_slots.end())
    {
      fg_color = the_config.get<sf::Color>("text_highlight_color");
      selection_order = (slot_iter - pImpl->selected_slots.begin()) + 1;
    }

    // 2. Display the selection order.
    if (selection_order != 0)
    {
      std::stringstream selection_number;
      selection_number << "[" << selection_order << "]" << std::endl;
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(the_config.get<unsigned int>("text_mono_default_size"));
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

      item_char = pImpl->get_character(slot_number);

      slot_id << item_char << ":";
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(the_config.get<unsigned int>("text_mono_default_size"));
      render_text.setString(slot_id.str());
      render_text.setPosition(text_coord_x + 55, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }

    // 4. Display the tile representing the item.
    the_tilesheet.getTexture().setSmooth(true);
    draw_thing(thing, texture,
               Vec2f(static_cast<float>(text_coord_x + 75), static_cast<float>(text_coord_y)),
               static_cast<unsigned int>(line_spacing_y - 1), false, frame);
    the_tilesheet.getTexture().setSmooth(false);

    unsigned int wield_location;
    WearLocation wear_location;
    bool wielding = pImpl->viewed->is_wielding(thing, wield_location);
    bool wearing = pImpl->viewed->has_equipped(thing, wear_location);

    // 5. TODO: Display "worn" or "equipped" icon if necessary.
    if (wielding)
    {
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(the_config.get<unsigned int>("text_mono_default_size"));
      render_text.setString("W");
      render_text.setPosition(text_coord_x + 11, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }
    else if (wearing)
    {
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(the_config.get<unsigned int>("text_mono_default_size"));
      render_text.setString("E");
      render_text.setPosition(text_coord_x + 11, text_coord_y);
      render_text.setColor(fg_color);
      texture.draw(render_text);
    }

    // 6. Display the item name.
    std::stringstream item_name;
    if (selection_order == 1)
    {
      unsigned int max_quantity = thing->get_quantity();
      if ((max_quantity > 1) && (pImpl->selected_quantity < max_quantity))
      {
        item_name << "(Sel: " << pImpl->selected_quantity << ") ";
      }
    }

    item_name << thing->get_identifying_string(ArticleChoice::Indefinite, UsePossessives::No);

    if (wielding)
    {
      item_name << " (" << pImpl->viewed->get_bodypart_description(BodyPart::Hand, wield_location) << ")";
    }
    else if (wearing)
    {
      item_name << " (" << pImpl->viewed->get_bodypart_description(wear_location.part, wear_location.number) << ")";
    }

    render_text.setFont(the_default_font);
    render_text.setCharacterSize(the_config.get<unsigned int>("text_default_size"));
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
    separator_line.setSize(Vec2f(static_cast<float>(pane_dims.width - 25), 1.0f));
    separator_line.setFillColor(the_config.get<sf::Color>("window_border_color"));
    texture.draw(separator_line);

    text_coord_y += item_spacing_y;
  }

  // Figure out the title.
  // TODO: Might want to define a specific "get_inventory_name()" method
  //       for Thing that defaults to "XXXX's inventory" but can be
  //       overridden to say stuff like "Things on the floor".
  sf::String title_string = pImpl->viewed->get_possessive() + L" inventory";
  title_string[0] = toupper(title_string[0]);
  set_text(title_string);
  return;
}

void InventoryArea::draw_thing(ThingId thing, 
                               sf::RenderTarget& target, 
                               Vec2f target_coords, 
                               unsigned int target_size, 
                               bool use_lighting, 
                               int frame)
{
  MapTile* root_tile = thing->get_maptile();

  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  sf::RectangleShape rectangle;
  sf::IntRect texture_coords;

  if (target_size == 0)
  {
    target_size = the_config.get<unsigned int>("map_tile_size");
  }

  unsigned int tile_size = the_config.get<unsigned int>("map_tile_size");

  Vec2u tile_coords = thing->get_tile_sheet_coords(frame);
  texture_coords.left = tile_coords.x * tile_size;
  texture_coords.top = tile_coords.y * tile_size;
  texture_coords.width = tile_size;
  texture_coords.height = tile_size;

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile->get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  rectangle.setPosition(target_coords);
  rectangle.setSize(Vec2f(static_cast<float>(target_size),
                                 static_cast<float>(target_size)));
  rectangle.setTexture(&(the_tilesheet.getTexture()));
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

  target.draw(rectangle);
}
