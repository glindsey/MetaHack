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
  Impl(std::vector<ThingId>& s, unsigned int& q)
    : use_capitals(false),
      viewed_container_ptr(nullptr),
      inventory_type(InventoryType::Inside),
      selected_things(s),
      selected_quantity(q) {}

  /// If true this inventory enumerates Things using capital letters.
  /// If false it uses lower-case letters.
  bool use_capitals;

  /// Container whose contents (or surroundings) are currently being viewed.
  Container* viewed_container_ptr;

  /// Type indicating whether we're looking at contents or surroundings.
  InventoryType inventory_type;

  /// Reference to Vector of Things selected to perform an action on.
  std::vector<ThingId>& selected_things;

  /// Reference to quantity of topmost selected item.
  unsigned int& selected_quantity;
};

InventoryArea::InventoryArea(sf::IntRect dimensions,
                             std::vector<ThingId>& selected_things,
                             unsigned int& selected_quantity)
  : GUIPane(dimensions),
    impl(new Impl(selected_things, selected_quantity))
{}

InventoryArea::~InventoryArea()
{
  //dtor
}

void InventoryArea::set_capital_letters(bool use_capitals)
{
  impl->use_capitals = use_capitals;
}

bool InventoryArea::get_capital_letters()
{
  return impl->use_capitals;
}

void InventoryArea::set_inventory_type(InventoryType type)
{
  impl->inventory_type = type;
}

InventoryType InventoryArea::get_inventory_type()
{
  return impl->inventory_type;
}

Container& InventoryArea::get_viewed_container()
{
  return *(impl->viewed_container_ptr);
}

void InventoryArea::set_viewed_container(Container& container)
{
  impl->viewed_container_ptr = &container;
}

void InventoryArea::toggle_selection(unsigned int selection)
{
  // Get a reference to the location we're referring to.
  Container& location = *(impl->viewed_container_ptr);

  // Get a reference to the location's location!
  Container& around = TF.get_container(location.get_location_id());

  Inventory* inventory_ptr = &(TF.get_limbo().get_inventory());

  switch (impl->inventory_type)
  {
  case InventoryType::Inside:
    inventory_ptr = &(location.get_inventory());
    break;
  case InventoryType::Around:
    inventory_ptr = &(around.get_inventory());
    break;
  default:
    break;
  }

  InventorySlot slot = static_cast<InventorySlot>(selection);

  if (inventory_ptr->contains(slot))
  {
    ThingId id = inventory_ptr->get(slot);

    std::vector<ThingId>::iterator iter;
    iter = std::find(impl->selected_things.begin(),
                     impl->selected_things.end(),
                     id);
    if ( iter == impl->selected_things.end())
    {
      //TRACE("Adding thing %u to selected things",
      //      static_cast<unsigned int>(id));
      impl->selected_things.push_back(id);
    }
    else
    {
      //TRACE("Removing thing %u from selected things",
      //      static_cast<unsigned int>(id));
      impl->selected_things.erase(iter);
    }
  }
}

ThingId InventoryArea::get_thingid(unsigned int selection)
{
  // Get a reference to the location we're referring to.
  Container& location = *(impl->viewed_container_ptr);

  // Get a reference to the location's location!
  Container& around = TF.get_container(location.get_location_id());

  Inventory* inventory_ptr = &(TF.get_limbo().get_inventory());

  switch (impl->inventory_type)
  {
  case InventoryType::Inside:
    inventory_ptr = &(location.get_inventory());
    break;
  case InventoryType::Around:
    inventory_ptr = &(around.get_inventory());
    break;
  default:
    break;
  }

  InventorySlot slot = static_cast<InventorySlot>(selection);

  if (inventory_ptr->contains(slot))
  {
    return inventory_ptr->get(slot);
  }
  else
  {
    TRACE("Requested non-existent inventory slot %u, returning limbo!",
          selection);
    return TF.limbo_id;
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
  Container& location = *(impl->viewed_container_ptr);

  // Get a reference to the location's location!
  Container& around = TF.get_container(location.get_location_id());

  // Start at the top and work down.
  float text_coord_x = text_offset_x;
  float text_coord_y = text_offset_y + (line_spacing_y * 1.5);

  Inventory* inventory_ptr = &(TF.get_limbo().get_inventory());

  switch (impl->inventory_type)
  {
  case InventoryType::Inside:
    inventory_ptr = &(location.get_inventory());
    break;
  case InventoryType::Around:
    inventory_ptr = &(around.get_inventory());
    break;
  default:
    break;
  }

  /// @todo At the moment this does not split lines that are too long, instead
  ///       truncating them at the edge of the box.  This must be fixed.
  /// @todo Also need to display some details about the item, such as whether it
  ///       is equipped, what magions are equipped to it, et cetera.
  for (ThingMapBySlot::const_iterator iter = inventory_ptr->by_slot_cbegin();
       iter != inventory_ptr->by_slot_cend(); ++iter)
  {
    InventorySlot slot = (*iter).first;
    ThingId id = (*iter).second;
    unsigned int slot_number = static_cast<unsigned int>(slot);
    sf::Text render_text;

    // 1. Figure out whether this is selected or not, and set FG color.
    sf::Color fg_color = Settings.text_color;
    unsigned int selection_order = 0;
    std::vector<ThingId>::iterator thing_iter;
    thing_iter = std::find(impl->selected_things.begin(),
                           impl->selected_things.end(),
                           id);

    if (thing_iter != impl->selected_things.end())
    {
      fg_color = Settings.text_highlight_color;
      selection_order = (thing_iter - impl->selected_things.begin()) + 1;
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

      if (impl->use_capitals)
      {
        item_char = '@' + static_cast<char>(slot_number);
      }
      else
      {
        item_char = '`' + static_cast<char>(slot_number);
      }

      slot_id << item_char << ":";
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(Settings.text_mono_default_size);
      render_text.setString(slot_id.str());
      render_text.setPosition(text_coord_x + 35, text_coord_y);
      render_text.setColor(fg_color);
      bg_texture.draw(render_text);
    }

    // 4. Display the tile representing the item.
    Thing& thing = TF.get(id);

    the_tile_sheet.getTexture().setSmooth(true);
    thing.draw_to(bg_texture,
                  sf::Vector2f(text_coord_x + 55, text_coord_y),
                  line_spacing_y - 1, false, frame);
    the_tile_sheet.getTexture().setSmooth(false);

    // 5. TODO: Display "worn" or "equipped" icon if necessary.
    // 5a. First, the inventory location must be an Entity.
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

    // 6. Display the item name.
    std::stringstream item_name;
    if (selection_order == 1)
    {
      unsigned int max_quantity = thing.get_quantity();
      if ((max_quantity > 1) && (impl->selected_quantity < max_quantity))
      {
        item_name << "(Sel: " << impl->selected_quantity << ") ";
      }
    }

    item_name << thing.get_indef_name();
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
  sf::String title_string;

  switch (impl->inventory_type)
  {
  case InventoryType::Inside:
    title_string = location.get_possessive() + " inventory";
    break;

  case InventoryType::Around:
    title_string = "Things at " + location.get_possessive() + " location";
    break;

  default:
    title_string = "Limbo (Unknown InventoryType!)";
    break;
  }

  title_string[0] = toupper(title_string[0]);
  return title_string;
}
