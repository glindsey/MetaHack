#include "InventoryArea.h"

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "Inventory.h"
#include "MapTile.h"
#include "Thing.h"
#include "ThingFactory.h"
#include "TileSheet.h"

#include <string>

struct InventoryArea::Impl
{
  Impl(std::vector<ThingId>& s)
    : focus(false),
      use_capitals(false),
      font_size(18), ///< @todo move to ConfigSettings
      viewed_container_ptr(nullptr),
      inventory_type(InventoryType::Inside),
      selected_things(s) {}

  /// Boolean indicating whether this area has the focus.
  bool focus;

  /// If true this inventory enumerates Things using capital letters.
  /// If false it uses lower-case letters.
  bool use_capitals;

  unsigned int font_size;
  sf::IntRect dims;

  /// Container whose contents (or surroundings) are currently being viewed.
  Container* viewed_container_ptr;

  /// Type indicating whether we're looking at contents or surroundings.
  InventoryType inventory_type;

  std::unique_ptr<sf::RenderTexture> area_bg_texture;

  sf::RectangleShape area_bg_shape;

  /// Vector of Things selected to perform an action on.
  std::vector<ThingId>& selected_things;
};

InventoryArea::InventoryArea(sf::IntRect dimensions,
                             std::vector<ThingId>& selected_things)
  : impl(new Impl(selected_things))
{
  this->set_dimensions(dimensions);
}

InventoryArea::~InventoryArea()
{
  //dtor
}

void InventoryArea::set_focus(bool focus)
{
  impl->focus = focus;
}

bool InventoryArea::get_focus()
{
  return impl->focus;
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

sf::IntRect InventoryArea::get_dimensions()
{
  return impl->dims;
}

void InventoryArea::set_dimensions(sf::IntRect rect)
{
  impl->dims = rect;
  impl->area_bg_texture.reset(new sf::RenderTexture());
  impl->area_bg_texture->create(rect.width, rect.height);
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

EventResult InventoryArea::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

bool InventoryArea::render(sf::RenderTarget& target, int frame)
{
  int line_spacing_y = the_default_font.getLineSpacing(impl->font_size);
  int item_spacing_y = 4;

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear background texture.
  impl->area_bg_texture->clear(Settings.window_bg_color);

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

    std::string item_string;
    char item_char;

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
      // GSL: std::to_string is broken in the version of MinGW I'm currently
      //      using, so here's a horrible workaround!
      char buf[5];
      snprintf(buf, 4, "[%u]", selection_order);
      item_string.append(buf);
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(impl->font_size - 3);
      render_text.setString(item_string);
      render_text.setPosition(text_coord_x + 1, text_coord_y);
      render_text.setColor(fg_color);
      impl->area_bg_texture->draw(render_text);
    }

    // 3. Display the slot ID.
    if (slot_number < 27)
    {
      if (impl->use_capitals)
      {
        item_char = '@' + static_cast<char>(slot_number);
      }
      else
      {
        item_char = '`' + static_cast<char>(slot_number);
      }

      item_string.clear();
      item_string.push_back(item_char);
      item_string.push_back(':');
      render_text.setFont(the_default_mono_font);
      render_text.setCharacterSize(impl->font_size - 3);
      render_text.setString(item_string);
      render_text.setPosition(text_coord_x + 35, text_coord_y);
      render_text.setColor(fg_color);
      impl->area_bg_texture->draw(render_text);
    }

    // 4. Display the tile representing the item.
    Thing& thing = TF.get(id);

    the_tile_sheet.getTexture().setSmooth(true);
    thing.draw_to(*(impl->area_bg_texture.get()),
                  sf::Vector2f(text_coord_x + 55, text_coord_y),
                  line_spacing_y - 1, false, frame);
    the_tile_sheet.getTexture().setSmooth(false);

    // 5. Display the item name.
    item_string.clear();
    item_string = thing.get_indef_name();
    render_text.setFont(the_default_font);
    render_text.setCharacterSize(impl->font_size);
    render_text.setString(item_string);
    render_text.setPosition(text_coord_x + 60 + line_spacing_y,
                            text_coord_y + 1);
    render_text.setColor(fg_color);
    impl->area_bg_texture->draw(render_text);

    if (text_coord_y > impl->dims.height) break;
    text_coord_y += line_spacing_y + item_spacing_y;

    // 6. Display a nice separator line.
    sf::RectangleShape separator_line;
    separator_line.setPosition(text_coord_x + 10, text_coord_y);
    separator_line.setSize(sf::Vector2f(impl->dims.width - 25, 1));
    separator_line.setFillColor(Settings.window_border_color);
    impl->area_bg_texture->draw(separator_line);

    text_coord_y += item_spacing_y;
  }

  // Draw a little window title in the upper-left corner.
  sf::RectangleShape title_rect;
  sf::Text title_text;
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
  title_text.setString(title_string);
  title_text.setFont(the_default_bold_font);
  title_text.setCharacterSize(impl->font_size);

  title_rect.setFillColor(Settings.window_bg_color);
  title_rect.setOutlineColor(impl->focus ?
                            Settings.window_focused_border_color :
                            Settings.window_border_color);
  title_rect.setOutlineThickness(Settings.window_border_width);
  title_rect.setPosition(sf::Vector2f(0, 0));
  title_rect.setSize(sf::Vector2f(impl->dims.width,
                                  line_spacing_y + (text_offset_y * 2)));

  impl->area_bg_texture->draw(title_rect);

  location.draw_to(*(impl->area_bg_texture.get()),
                   sf::Vector2f(text_offset_x, text_offset_y),
                   line_spacing_y - 1, false, frame);


  title_text.setColor(Settings.text_shadow_color);
  title_text.setPosition(sf::Vector2f(text_offset_x + line_spacing_y,
                                      text_offset_y - 1));
  impl->area_bg_texture->draw(title_text);
    title_text.setColor(Settings.text_color);
  title_text.setPosition(sf::Vector2f(text_offset_x + line_spacing_y + 2,
                                      text_offset_y + 1));
  impl->area_bg_texture->draw(title_text);

  // Render to the texture.
  impl->area_bg_texture->display();

  // Draw the rectangle.
  impl->area_bg_shape.setPosition(sf::Vector2f(impl->dims.left,
                                              impl->dims.top));
  impl->area_bg_shape.setSize(sf::Vector2f(impl->dims.width,
                                          impl->dims.height));
  impl->area_bg_shape.setTexture(&(impl->area_bg_texture->getTexture()));
  impl->area_bg_shape.setTextureRect(sf::IntRect(0, 0,
                                                impl->dims.width,
                                                impl->dims.height));
  impl->area_bg_shape.setOutlineColor(impl->focus ?
                                      Settings.window_focused_border_color :
                                      Settings.window_border_color);
  impl->area_bg_shape.setOutlineThickness(Settings.window_border_width);

  target.setView(sf::View(sf::FloatRect(0, 0,
                          target.getSize().x,
                          target.getSize().y)));

  target.draw(impl->area_bg_shape);

  return true;
}
