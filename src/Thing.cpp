#include "Thing.h"

#include <iomanip>
#include <sstream>
#include <vector>

#include "App.h"
#include "ConfigSettings.h"
#include "Container.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
#include "IsType.h"
#include "Map.h"
#include "MapTile.h"
#include "MessageLog.h"
#include "ThingFactory.h"
#include "TileSheet.h"

struct Thing::Impl
{
  ThingId thing_id;
  ThingId location_id;
  int physical_size;
  int physical_mass;
  std::string proper_name;
  Inventory inventory;
};

// Static member initialization.
float Thing::font_line_to_point_ratio_ = 0.0f;
sf::Color const Thing::wall_outline_color_ = sf::Color(255, 255, 255, 64);

// Static methods
void Thing::initialize_font_sizes()
{
  // Ratio to multiply by to get desired character point size.
  font_line_to_point_ratio_ = 100.0f / static_cast<float>(the_default_font.getLineSpacing(100));
}

Thing::Thing()
  : impl(new Impl())
{
  impl->thing_id = static_cast<ThingId>(0);
  impl->location_id = static_cast<ThingId>(0);
  impl->physical_size = 1;
  impl->physical_mass = 1;
}

Thing::Thing(const Thing& original)
  : impl(new Impl())
{
  impl->physical_size = original.get_size();
  impl->physical_mass = original.get_mass();
  impl->proper_name = original.get_proper_name();
  // GSL NOTE: Inventory is NOT copied!
}

Thing::~Thing()
{
  //dtor
}

ThingId Thing::get_id() const
{
  return impl->thing_id;
}

ThingId Thing::get_location_id() const
{
  return impl->location_id;
}

ThingId Thing::get_root_id() const
{
  if (this->get_location_id() != this->get_id())
  {
    Thing& parentThing = TF.get(this->get_location_id());
    return parentThing.get_root_id();
  }
  else
  {
    return this->get_id();
  }
}

ThingId Thing::get_owner_id() const
{
  ThingId location_id = get_location_id();
  Container& location = TF.get_container(location_id);
  if (location.is_maptile() ||
      location.is_entity() ||
      location_id == this->get_id())
  {
    return location_id;
  }
  else
  {
    return location.get_owner_id();
  }
}

MapId Thing::get_map_id() const
{
  Container& location = TF.get_container(get_location_id());
  if (location.is_maptile())
  {
    MapTile& tile = TF.get_tile(get_location_id());
    return tile.get_map_id();
  }
  else
  {
    return MapFactory::null_map_id;
  }
}

std::string Thing::get_proper_name() const
{
  return impl->proper_name;
}

void Thing::set_proper_name(std::string name)
{
  impl->proper_name = name;
}

void Thing::set_single_size(int s)
{
  impl->physical_size = s;
}

void Thing::set_single_mass(int mass)
{
  impl->physical_mass = mass;
}

int Thing::get_single_size() const
{
  return impl->physical_size;
}

int Thing::get_single_mass() const
{
  return impl->physical_mass;
}

std::string Thing::get_name() const
{
  // If the thing is YOU, use YOU.
  if (impl->thing_id == TF.get_player_id())
  {
    return "you";
  }

  std::string name;
  Container& owner = TF.get_container(get_owner_id());

  // If the Thing has a proper name, use that.
  if (!get_proper_name().empty())
  {
    if (owner.is_entity())
    {
      name = owner.get_possessive() + " ";
    }

    name += get_proper_name();
  }
  else
  {
    if (owner.is_entity())
    {
      name = owner.get_possessive() + " ";
    }
    else
    {
      name = "the ";
    }

    name += get_description();
  }

  return name;
}

std::string Thing::get_indef_name() const
{
  // If the thing is YOU, use YOU.
  if (impl->thing_id == TF.get_player_id())
  {
    return "you";
  }

  std::string name;

  // If the Thing has a proper name, use that.
  if (!get_proper_name().empty())
  {
    name = get_proper_name();
  }
  else
  {
    std::string description = get_description();
    name = getIndefArt(description) + " " + description;
  }

  return name;
}

std::string const& Thing::choose_verb(std::string const& verb12,
                                     std::string const& verb3) const
{
  if (get_id() == TF.get_player_id())
  {
    return verb12;
  }
  else
  {
    return verb3;
  }
}

int Thing::get_size() const
{
  return get_single_size();
}

int Thing::get_mass() const
{
  return get_single_mass();
}

std::string Thing::get_plural() const
{
  return get_description() + "s";
}

std::string const& Thing::get_subject_pronoun()
{
  return getSubjPro(Gender::None);
}

std::string const& Thing::get_object_pronoun()
{
  return getObjPro(Gender::None);
}

std::string const& Thing::get_reflexive_pronoun()
{
  return getRefPro(Gender::None);
}

std::string const& Thing::get_possessive_adjective()
{
  return getPossAdj(Gender::None);
}

std::string const& Thing::get_possessive_pronoun()
{
  return getPossPro(Gender::None);
}

std::string Thing::get_possessive() const
{
  static std::string const your = std::string("your");

  if (get_id() == TF.get_player_id())
  {
    return your;
  }
  else
  {
    return get_name() + "'s";
  }
}

sf::Vector2u Thing::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(0, 0);  // The "unknown thing" tile
}

void Thing::add_vertices_to(sf::VertexArray& vertices,
                            bool use_lighting,
                            int frame)
{
  static sf::Vertex new_vertex;
  float ts = static_cast<float>(Settings.map_tile_size);
  float ts2 = ts / 2.0;

  ThingId root_id = this->get_root_id();
  if (!TF.is_a_tile(root_id))
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  MapTile& root_tile = dynamic_cast<MapTile&>(TF.get(root_id));
  sf::Vector2i const& coords = root_tile.get_coords();

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile.get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  sf::Vector2f location(coords.x * ts, coords.y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);
  sf::Vector2u tile_coords = this->get_tile_sheet_coords(frame);
  sf::Vector2f texNW = sf::Vector2f(tile_coords.x * ts,
                                    tile_coords.y * ts);

  new_vertex.color = thing_color;
  new_vertex.position = vNW;
  new_vertex.texCoords = texNW;
  vertices.append(new_vertex);

  new_vertex.position = vNE;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts,
                                      texNW.y);
  vertices.append(new_vertex);

  new_vertex.position = vSE;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts,
                                      texNW.y + ts);
  vertices.append(new_vertex);

  new_vertex.position = vSW;
  new_vertex.texCoords = sf::Vector2f(texNW.x,
                                      texNW.y + ts);
  vertices.append(new_vertex);
}

void Thing::draw_to(sf::RenderTexture& target,
                    sf::Vector2f target_coords,
                    unsigned int target_size,
                    bool use_lighting,
                    int frame)
{
  ThingId root_id = this->get_root_id();
  if (!TF.is_a_tile(root_id))
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  MapTile& root_tile = dynamic_cast<MapTile&>(TF.get(root_id));
  sf::RectangleShape rectangle;
  sf::IntRect texture_coords;

  if (target_size == 0)
  {
    target_size = Settings.map_tile_size;
  }

  float tile_size = static_cast<float>(Settings.map_tile_size);

  sf::Vector2u tile_coords = this->get_tile_sheet_coords(frame);
  texture_coords.left = tile_coords.x * tile_size;
  texture_coords.top = tile_coords.y * tile_size;
  texture_coords.width = tile_size;
  texture_coords.height = tile_size;

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile.get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  rectangle.setPosition(target_coords);
  rectangle.setSize(sf::Vector2f(target_size, target_size));
  rectangle.setTexture(&(the_tile_sheet.getTexture()));
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

  target.draw(rectangle);
}

bool Thing::is_container() const
{
  return isType(this, Container const);
}

bool Thing::is_entity() const
{
  return isType(this, Entity const);
}

bool Thing::is_maptile() const
{
  return isType(this, MapTile const);
}

bool Thing::move_into(ThingId new_location_id)
{
  return move_into(TF.get_container(new_location_id));
}

bool Thing::move_into(Container& new_location)
{
  Container& old_location = TF.get_container(this->get_location_id());

  if (can_be_moved())
  {
    if (new_location.can_contain(*this))
    {
      if (new_location.get_inventory().add(impl->thing_id))
      {
        old_location.get_inventory().remove(impl->thing_id);
        impl->location_id = new_location.get_id();
        return true;
      }
    }
  }

  return false;
}

bool Thing::can_be_moved() const
{
  return true;
}

bool Thing::can_be_activated_by(Entity const& entity) const
{
  return false;
}

bool Thing::can_be_drank_by(Entity const& entity) const
{
  return false;
}

bool Thing::can_be_eaten_by(Entity const& entity) const
{
  return false;
}

bool Thing::can_be_put_into(Thing const& container) const
{
  return true;
}

bool Thing::can_be_taken_out() const
{
  return true;
}

bool Thing::can_be_read_by(Entity const& entity) const
{
  return false;
}

bool Thing::can_be_deequipped_from(Thing const& thing) const
{
  return false;
}

bool Thing::can_be_thrown_by(Entity const& entity) const
{
  return true;
}

bool Thing::can_be_wielded_by(Entity const& entity) const
{
  return true;
}

bool Thing::can_be_equipped_on(Thing const& thing) const
{
  return false;
}

bool Thing::can_be_fired_by(Entity const& entity) const
{
  return false;
}

bool Thing::can_be_mixed_with(Thing const& thing) const
{
  return false;
}

bool Thing::do_process()
{
  return true;
}

void Thing::gather_thing_ids(std::vector<ThingId>& ids)
{
  // Not a Container, so just add this one ID.
  ids.push_back(impl->thing_id);
}

bool Thing::do_action_activated_by(Entity& entity)
{
  return false;
}

bool Thing::do_action_collided_with(Thing& thing)
{
  return true;
}

bool Thing::do_action_drank_by(Entity& entity)
{
  return false;
}

bool Thing::do_action_dropped_by(Entity& entity)
{
  return true;
}

bool Thing::do_action_eaten_by(Entity& entity)
{
  return false;
}

bool Thing::do_action_picked_up_by(Entity& entity)
{
  return true;
}

bool Thing::do_action_put_into(Thing& container)
{
  return true;
}

bool Thing::do_action_take_out()
{
  return true;
}

ActionResult Thing::do_action_read_by(Entity& entity)
{
  return ActionResult::Failure;
}

bool Thing::do_action_deequipped_from(Thing& thing)
{
  return true;
}

bool Thing::do_action_thrown_by(Entity& thing, Direction direction)
{
  return true;
}

bool Thing::do_action_equipped_onto(Thing& thing)
{
  return false;
}

bool Thing::do_action_wielded_by(Entity& entity)
{
  return true;
}

bool Thing::do_action_fired_by(Entity& entity, Direction direction)
{
  return false;
}

char const* Thing::get_thing_type() const
{
  return typeid(*this).name();
}

bool Thing::is_liquid_carrier() const
{
  return false;
}

void Thing::set_id(ThingId id)
{
  impl->thing_id = id;
}

void Thing::set_location_id(ThingId target)
{
  impl->location_id = target;
}
