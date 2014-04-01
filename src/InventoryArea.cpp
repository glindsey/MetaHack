#include "InventoryArea.h"

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "Inventory.h"
#include "MapTile.h"
#include "Thing.h"
#include "ThingFactory.h"
#include "TileSheet.h"

#include <sstream>
#include <string>

struct InventoryArea::Impl
{
  /// Thing whose contents (or surroundings) are currently being viewed.
  std::weak_ptr<Thing> viewed_ptr;

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
  : GUIPane(dimensions),
    impl(new Impl())
{}

InventoryArea::~InventoryArea()
{
  //dtor
}

std::weak_ptr<Thing> InventoryArea::get_viewed() const
{
  return impl->viewed_ptr;
}

void InventoryArea::set_viewed(std::weak_ptr<Thing> thing)
{
  impl->viewed_ptr = thing;
  impl->selected_slots.clear();
}

void InventoryArea::toggle_selection(InventorySlot selection)
{
  auto viewed_ptr = impl->viewed_ptr.lock();

  if (!viewed_ptr)
  {
    return;
  }

  Inventory& inventory = viewed_ptr->get_inventory();

  if (inventory.contains(selection))
  {
    auto iter = std::find(std::begin(impl->selected_slots),
                          std::end(impl->selected_slots),
                          selection);
    if ( iter == std::end(impl->selected_slots))
    {
      //TRACE("Adding thing %u to selected things",
      //      static_cast<unsigned int>(id));
      impl->selected_slots.push_back(selection);
    }
    else
    {
      //TRACE("Removing thing %u from selected things",
      //      static_cast<unsigned int>(id));
      impl->selected_slots.erase(iter);
    }

    reset_selected_quantity();
  }
}

unsigned int InventoryArea::get_selected_slot_count() const
{
  return impl->selected_slots.size();
}

std::vector<InventorySlot> const& InventoryArea::get_selected_slots()
{
  return impl->selected_slots;
}

std::vector<std::weak_ptr<Thing>> InventoryArea::get_selected_things()
{
  std::vector<std::weak_ptr<Thing>> things;

  auto viewed_ptr = impl->viewed_ptr.lock();

  if (viewed_ptr)
  {
    Inventory& inventory = viewed_ptr->get_inventory();

    for (auto iter = std::begin(impl->selected_slots);
              iter != std::end(impl->selected_slots);
              ++iter)
    {
      std::weak_ptr<Thing> thing = inventory.get(*iter);
      things.push_back(thing);
    }
  }

  return things;
}

void InventoryArea::clear_selected_slots()
{
  impl->selected_slots.clear();
}

unsigned int InventoryArea::get_selected_quantity() const
{
  return impl->selected_quantity;
}

unsigned int InventoryArea::get_max_quantity() const
{
  unsigned int result;

  auto viewed_ptr = impl->viewed_ptr.lock();

  if (!viewed_ptr)
  {
    return 0;
  }

  Inventory& inventory = viewed_ptr->get_inventory();

  if (impl->selected_slots.size() == 0)
  {
    result = 0;
  }
  else
  {
    auto thing = inventory.get(impl->selected_slots[0]).lock();

    if (!thing)
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
  impl->selected_quantity = get_max_quantity();
  return impl->selected_quantity;
}

bool InventoryArea::set_selected_quantity(unsigned int amount)
{
  if (amount > 0)
  {
    unsigned int maximum = get_max_quantity();
    if (amount <= maximum)
    {
      impl->selected_quantity = amount;
      return true;
    }
  }
  return false;
}

bool InventoryArea::inc_selected_quantity()
{
  unsigned int maximum = get_max_quantity();
  if (impl->selected_quantity < maximum)
  {
    ++(impl->selected_quantity);
    return true;
  }
  return false;
}

bool InventoryArea::dec_selected_quantity()
{
  if (impl->selected_quantity > 1)
  {
    --(impl->selected_quantity);
    return true;
  }
  return false;
}

std::weak_ptr<Thing> InventoryArea::get_thing(InventorySlot selection)
{
  auto viewed_ptr = impl->viewed_ptr.lock();

  if (!viewed_ptr)
  {
    return std::weak_ptr<Thing>();
  }

  Inventory& inventory = viewed_ptr->get_inventory();

  if (inventory.contains(selection))
  {
    return inventory.get(selection);
  }
  else
  {
    TRACE("Requested non-existent inventory slot %u, returning nullptr!",
          static_cast<unsigned int>(selection));
    return std::weak_ptr<Thing>();
  }
}

EventResult InventoryArea::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

std::string InventoryArea::render_contents(int frame)
{
  // Dimensions of the pane.
  sf::IntRect pane_dims = get_dimensions();

  // Our render texture.
  sf::RenderTexture& bg_texture = get_bg_texture();

  int line_spacing_y = the_default_font.getLineSpacing(Settings.text_default_size);
  int item_spacing_y = 4;

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Get a reference to the location we're referring to.
  auto viewed_ptr = impl->viewed_ptr.lock();
  if (!viewed_ptr)
  {
    return "Invalid Viewed Object!";
  }

  // Start at the top and work down.
  float text_coord_x = text_offset_x;
  float text_coord_y = text_offset_y + (line_spacing_y * 1.5);

  Inventory& inventory = viewed_ptr->get_inventory();

  /// @todo At the moment this does not split lines that are too long, instead
  ///       truncating them at the edge of the box.  This must be fixed.
  /// @todo Also need to display some details about the item, such as whether it
  ///       is equipped, what magicules are equipped to it, et cetera.
  auto& things = inventory.get_things();

  for (auto iter = things.cbegin();
       iter != things.cend(); ++iter)
  {
    auto& slot = (*iter).first;
    auto& thing = (*iter).second;
    unsigned int slot_number = static_cast<unsigned int>(slot);
    sf::Text render_text;

    // 1. Figure out whether this is selected or not, and set FG color.
    sf::Color fg_color = Settings.text_color;
    unsigned int selection_order = 0;
    auto slot_iter = std::find(impl->selected_slots.begin(),
                                impl->selected_slots.end(),
                                slot);

    if (slot_iter != impl->selected_slots.end())
    {
      fg_color = Settings.text_highlight_color;
      selection_order = (slot_iter - impl->selected_slots.begin()) + 1;
    }

    // 2. Display the selection order.
    if (selection_order != 0)
    {
      std::stringstream selection_number;
      selection_number << "[" << selection_order << "]" << std::endl;
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(Settings.text_mono_default_size);
      render_text.setString(selection_number.str());
      render_text.setPosition(text_coord_x + 1, text_coord_y);
      render_text.setColor(fg_color);
      bg_texture.draw(render_text);
    }

    // 3. Display the slot ID.
    if (slot_number < 27)
    {
      std::stringstream slot_id;
      char item_char;

      item_char = impl->get_character(slot_number);

      slot_id << item_char << ":";
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(Settings.text_mono_default_size);
      render_text.setString(slot_id.str());
      render_text.setPosition(text_coord_x + 35, text_coord_y);
      render_text.setColor(fg_color);
      bg_texture.draw(render_text);
    }

    // 4. Display the tile representing the item.
    the_tile_sheet.getTexture().setSmooth(true);
    thing->draw_to(bg_texture,
                   sf::Vector2f(text_coord_x + 55, text_coord_y),
                   line_spacing_y - 1, false, frame);
    the_tile_sheet.getTexture().setSmooth(false);

    // 5. TODO: Display "worn" or "equipped" icon if necessary.
    // 5a. First, the inventory location must be an Entity.
    #if 0
    if (isType(&location, Entity))
    {
      Entity& location_entity = dynamic_cast<Entity&>(location);
      if (location_entity.is_wielding(id))
      {
        // TODO: draw wielding icon
      }
      else if (location_entity.has_equipped(id))
      {
        // TODO: draw equipped icon
      }
    }
    #endif

    // 6. Display the item name.
    std::stringstream item_name;
    if (selection_order == 1)
    {
      unsigned int max_quantity = thing->get_quantity();
      if ((max_quantity > 1) && (impl->selected_quantity < max_quantity))
      {
        item_name << "(Sel: " << impl->selected_quantity << ") ";
      }
    }

    item_name << thing->get_indef_name();
    render_text.setFont(the_default_font);
    render_text.setCharacterSize(Settings.text_default_size);
    render_text.setString(item_name.str());
    render_text.setPosition(text_coord_x + 60 + line_spacing_y,
                            text_coord_y + 1);
    render_text.setColor(fg_color);
    bg_texture.draw(render_text);

    if (text_coord_y > pane_dims.height) break;
    text_coord_y += line_spacing_y + item_spacing_y;

    // 7. Display a nice separator line.
    sf::RectangleShape separator_line;
    separator_line.setPosition(text_coord_x + 10, text_coord_y);
    separator_line.setSize(sf::Vector2f(pane_dims.width - 25, 1));
    separator_line.setFillColor(Settings.window_border_color);
    bg_texture.draw(separator_line);

    text_coord_y += item_spacing_y;
  }

  // Figure out the title.
  // TODO: Might want to define a specific "get_inventory_name()" method
  //       for Thing that defaults to "XXXX's inventory" but can be
  //       overridden to say stuff like "Things on the floor".
  sf::String title_string = viewed_ptr->get_possessive() + " inventory";
  title_string[0] = toupper(title_string[0]);
  return title_string;
}
