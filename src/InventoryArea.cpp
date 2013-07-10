#include "InventoryArea.h"

#include "App.h"
#include "ErrorHandler.h"
#include "Inventory.h"
#include "MapTile.h"
#include "Thing.h"
#include "ThingFactory.h"

#include <string>

struct InventoryArea::Impl
{
  Impl(std::vector<ThingId>& s)
    : selected_things(s) {}

  bool focus;
  bool use_capitals;
  unsigned int font_size;
  sf::IntRect dims;
  sf::Color area_bg_color;
  ThingId viewed_id;
  InventoryType inventory_type;
  std::unique_ptr<sf::RenderTexture> area_bg_texture;
  sf::RectangleShape area_bg_shape;
  std::vector<ThingId>& selected_things;
};

InventoryArea::InventoryArea(sf::IntRect dimensions,
                             std::vector<ThingId>& selected_things)
  : impl(new Impl(selected_things))
{
  impl->focus = false;
  impl->font_size = 16;
  impl->area_bg_color = the_window_bg_color;
  impl->viewed_id = TF.limbo_id;
  impl->inventory_type = InventoryType::Inside;
  impl->use_capitals = false;

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

ThingId InventoryArea::get_viewed_id()
{
  return impl->viewed_id;
}

void InventoryArea::set_viewed_id(ThingId viewed_id)
{
  impl->viewed_id = viewed_id;
}

void InventoryArea::toggle_selection(unsigned int selection)
{
  // Get a reference to the location we're referring to.
  Thing& location = TF.get(impl->viewed_id);

  Inventory* inventory_ptr = &(TF.get_limbo().get_inventory());

  switch (impl->inventory_type)
  {
  case InventoryType::Inside:
    inventory_ptr = &(location.get_inventory());
    break;
  case InventoryType::Around:
    inventory_ptr = &(TF.get(location.get_location_id()).get_inventory());
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
  int line_spacing_y = std::max(the_default_font.getLineSpacing(impl->font_size),
                                MapTile::get_tile_size());

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear background texture.
  impl->area_bg_texture->clear(impl->area_bg_color);

  // Get a reference to the location we're referring to.
  Thing& location = TF.get(impl->viewed_id);

  // Start at the top and work down.
  float text_coord_x = text_offset_x;
  float text_coord_y = text_offset_y + (line_spacing_y * 1.5);

  sf::Text renderText;

  renderText.setFont(the_default_font);
  renderText.setCharacterSize(impl->font_size);
  renderText.setColor(sf::Color::White);

  Inventory* inventory_ptr = &(TF.get_limbo().get_inventory());

  switch (impl->inventory_type)
  {
  case InventoryType::Inside:
    inventory_ptr = &(location.get_inventory());
    break;
  case InventoryType::Around:
    inventory_ptr = &(TF.get(location.get_location_id()).get_inventory());
    break;
  default:
    break;
  }

  // TODO: At the moment this does not split lines that are too long, instead
  //       truncating them at the edge of the box.  This must be fixed.
  // TODO: Also need to display some details about the item, such as whether it
  //       is equipped, what magions are equipped to it, et cetera.
  for (ThingMapBySlot::const_iterator iter = inventory_ptr->by_slot_cbegin();
       iter != inventory_ptr->by_slot_cend(); ++iter)
  {
    InventorySlot slot = (*iter).first;
    ThingId id = (*iter).second;
    unsigned int slot_number = static_cast<unsigned int>(slot);

    std::string item_string;
    char item_char;

    // 1. Figure out whether this is selected or not, and set FG color.
    sf::Color fg_color = sf::Color::White;
    unsigned int selection_order = 0;
    std::vector<ThingId>::iterator thing_iter;
    thing_iter = std::find(impl->selected_things.begin(),
                           impl->selected_things.end(),
                           id);

    if (thing_iter != impl->selected_things.end())
    {
      fg_color = sf::Color::Yellow;
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
      renderText.setString(item_string);
      renderText.setPosition(text_coord_x - 1, text_coord_y - 1);
      renderText.setColor(sf::Color::Black);
      impl->area_bg_texture->draw(renderText);
      renderText.setPosition(text_coord_x + 1, text_coord_y + 1);
      renderText.setColor(fg_color);
      impl->area_bg_texture->draw(renderText);
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
      renderText.setString(item_string);
      renderText.setPosition(text_coord_x + 23, text_coord_y - 1);
      renderText.setColor(sf::Color::Black);
      impl->area_bg_texture->draw(renderText);
      renderText.setPosition(text_coord_x + 25, text_coord_y + 1);
      renderText.setColor(fg_color);
      impl->area_bg_texture->draw(renderText);
    }

    // 4. Display the tile representing the item.
    Thing& thing = TF.get(id);

    thing.draw_to(*(impl->area_bg_texture.get()),
                  sf::Vector2f(text_coord_x + 50, text_coord_y),
                  line_spacing_y - 1, false, frame);

    // 5. Display the item name.
    item_string.clear();
    item_string = thing.get_indef_name();
    renderText.setString(item_string);
    renderText.setPosition(text_coord_x + 50 + line_spacing_y,
                           text_coord_y - 1);
    renderText.setColor(sf::Color::Black);
    impl->area_bg_texture->draw(renderText);
    renderText.setPosition(text_coord_x + 52 + line_spacing_y,
                           text_coord_y + 1);
    renderText.setColor(fg_color);
    impl->area_bg_texture->draw(renderText);

    // 6. (TODO) Display a nice separator line.

    if (text_coord_y > impl->dims.height) break;
    text_coord_y += line_spacing_y;
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

  title_rect.setFillColor(impl->area_bg_color);
  title_rect.setOutlineColor(impl->focus ?
                            sf::Color::Yellow :
                            sf::Color::White);
  title_rect.setOutlineThickness(2.0f);
  title_rect.setPosition(sf::Vector2f(0, 0));
  title_rect.setSize(sf::Vector2f(impl->dims.width,
                                  line_spacing_y + (text_offset_y * 2)));

  impl->area_bg_texture->draw(title_rect);

  location.draw_to(*(impl->area_bg_texture.get()),
                   sf::Vector2f(text_offset_x, text_offset_y),
                   line_spacing_y - 1, false, frame);


  title_text.setColor(sf::Color::Black);
  title_text.setPosition(sf::Vector2f(text_offset_x + line_spacing_y,
                                      text_offset_y - 1));
  impl->area_bg_texture->draw(title_text);
    title_text.setColor(sf::Color::White);
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
                                     sf::Color::Yellow :
                                     sf::Color::White);
  impl->area_bg_shape.setOutlineThickness(2.0f);

  target.setView(sf::View(sf::FloatRect(0, 0,
                          target.getSize().x,
                          target.getSize().y)));

  target.draw(impl->area_bg_shape);

  return true;
}
