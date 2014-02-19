#include "Thing.h"

#include <iomanip>
#include <sstream>
#include <vector>

#include "App.h"
#include "ConfigSettings.h"
#include "Container.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
#include "IsType.h"
#include "LightSource.h"
#include "Map.h"
#include "MapTile.h"
#include "MessageLog.h"
#include "ThingFactory.h"
#include "TileSheet.h"

struct Thing::Impl
{
  ThingId thing_id;
  ThingId location_id;
  Qualities qualities;
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
  impl->qualities.physical_mass = 1;
}

Thing::Thing(const Thing& original)
  : impl(new Impl())
{
  impl->qualities.direction = original.get_facing_direction();
  impl->qualities.physical_mass = original.get_mass();
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

void Thing::set_facing_direction(Direction d)
{
  impl->qualities.direction = d;
}

Direction Thing::get_facing_direction() const
{
  return impl->qualities.direction;
}

void Thing::set_single_mass(int mass)
{
  impl->qualities.physical_mass = mass;
}

int Thing::get_single_mass() const
{
  return impl->qualities.physical_mass;
}

void Thing::set_magically_locked(bool locked)
{
  impl->qualities.magic_locked = locked;
}

void Thing::set_magically_autolocks(bool autolocks)
{
  impl->qualities.magic_autolocking = autolocks;
}

bool Thing::is_magically_locked() const
{
  return impl->qualities.magic_locked;
}

bool Thing::magically_autolocks() const
{
  return impl->qualities.magic_autolocking;
}

Thing::Qualities const* Thing::get_qualities_pointer() const
{
  return &(impl->qualities);
}

bool Thing::has_same_type_as(Thing const& other) const
{
  return (typeid(*this) == typeid(other));
}

bool Thing::has_same_qualities_as(Thing const& other) const
{
  int compare_result = memcmp(this->get_qualities_pointer(),
                              other.get_qualities_pointer(),
                              sizeof(Qualities));
  return (compare_result == 0);
}

std::string Thing::get_description() const
{
  /// @todo Implement adding adjectives.
  return _get_description();
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
  if (owner.is_entity())
  {
    name = owner.get_possessive() + " " + get_description();
  }
  else
  {
    name = "the "+ get_description();
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

  std::string description = get_description();
  name = getIndefArt(description) + " " + description;

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

int Thing::get_mass() const
{
  return get_single_mass();
}

unsigned int Thing::get_quantity() const
{
  return 1;
}

std::string Thing::get_plural() const
{
  return _get_description() + "s";
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
  Direction direction = this->get_facing_direction();
  if (direction == Direction::None)
  {
    return sf::Vector2u(4, 3); // The "unknown thing" tile
  }
  else
  {
    int x_pos = get_appropriate_4way_tile(this->get_facing_direction());
    return sf::Vector2u(x_pos, 3);  // The "unknown directional" tile
  }
}

void Thing::add_vertices_to(sf::VertexArray& vertices,
                            bool use_lighting,
                            int frame)
{
  sf::Vertex new_vertex;
  float ts = static_cast<float>(Settings.map_tile_size);
  float ts2 = ts * 0.5;

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
    if (TF.is_a_tile(this->get_id()))
    {
      sf::Color light = root_tile.get_light_level();
      thing_color.r = light.r * 0.8;
      thing_color.g = light.g * 0.8;
      thing_color.b = light.b * 0.8;
      thing_color.a = light.a * 0.8;
    }
    else
    {
      thing_color = root_tile.get_light_level();
    }
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

  TileSheet::add_vertices(vertices,
                          tile_coords, thing_color,
                          vNW, vNE, vSW, vSE);
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

bool Thing::is_opaque() const
{
  return true;
}

void Thing::light_up_surroundings()
{
  // Default behavior does nothing
}

void Thing::be_lit_by(LightSource& light)
{
  // Default behavior does nothing
}

bool Thing::move_into(ThingId new_location_id)
{
  return move_into(TF.get_container(new_location_id));
}

bool Thing::move_into(Container& new_location)
{
  Container& old_location = TF.get_container(this->get_location_id());

  if (is_movable())
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

bool Thing::is_movable() const
{
  return true;
}

bool Thing::usable_by(Entity const& entity) const
{
  return false;
}

bool Thing::drinkable_by(Entity const& entity) const
{
  return false;
}

bool Thing::edible_by(Entity const& entity) const
{
  return false;
}

bool Thing::readable_by(Entity const& entity) const
{
  return false;
}

bool Thing::miscible_with(Thing const& thing) const
{
  return false;
}

BodyPart Thing::equippable_on() const
{
  return BodyPart::Count;
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

bool Thing::perform_action_activated_by(Entity& entity)
{
  return _perform_action_activated_by(entity);
}

void Thing::perform_action_collided_with(Thing& thing)
{
  _perform_action_collided_with(thing);
}

bool Thing::perform_action_drank_by(Entity& entity)
{
  return _perform_action_drank_by(entity);
}

bool Thing::perform_action_dropped_by(Entity& entity)
{
  return _perform_action_dropped_by(entity);
}

bool Thing::perform_action_eaten_by(Entity& entity)
{
  return _perform_action_eaten_by(entity);
}

bool Thing::perform_action_picked_up_by(Entity& entity)
{
  return _perform_action_picked_up_by(entity);
}

bool Thing::perform_action_put_into(Thing& container)
{
  return _perform_action_put_into(container);
}

bool Thing::perform_action_take_out()
{
  return _perform_action_take_out();
}

ActionResult Thing::perform_action_read_by(Entity& entity)
{
  return _perform_action_read_by(entity);
}

void Thing::perform_action_attack_hits(Entity& entity)
{
  _perform_action_attack_hits(entity);
}

bool Thing::perform_action_thrown_by(Entity& entity, Direction direction)
{
  return _perform_action_thrown_by(entity, direction);
}

bool Thing::perform_action_deequipped_by(Entity& entity, WearLocation& location)
{
  if (this->is_magically_locked())
  {
    std::string message;
    message = entity.get_name() + " cannot take off " + this->get_name() +
              "; it is magically welded onto " +
              entity.get_possessive_adjective() + " " +
              entity.get_bodypart_description(location.part,
                                              location.number) + "!";
    the_message_log.add(message);
    return false;
  }
  else
  {
    return _perform_action_deequipped_by(entity, location);
  }
}

bool Thing::perform_action_equipped_by(Entity& entity, WearLocation& location)
{
  bool subclass_result = _perform_action_equipped_by(entity, location);

  if (subclass_result == true)
  {
    if (this->magically_autolocks())
    {
        this->set_magically_locked(true);
        std::string message;
        message = this->get_name() + " magically welds itself onto " +
                  entity.get_possessive() + " " +
                  entity.get_bodypart_description(location.part,
                                                  location.number) + "!";
        the_message_log.add(message);
    }
  }

  return subclass_result;
}

bool Thing::perform_action_unwielded_by(Entity& entity)
{
  if (this->is_magically_locked())
  {
    std::string message;
    message = entity.get_name() + " cannot unwield " + this->get_name() +
              "; it is magically welded onto " +
              entity.get_possessive_adjective() + " " +
              entity.get_bodypart_name(BodyPart::Hand) + "!";
    the_message_log.add(message);
    return false;
  }
  else
  {
    return _perform_action_unwielded_by(entity);
  }
}

bool Thing::perform_action_wielded_by(Entity& entity)
{
  bool subclass_result = _perform_action_wielded_by(entity);

  if (subclass_result == true)
  {
    if (this->magically_autolocks())
    {
      this->set_magically_locked(true);
      std::string message;
      message = this->get_name() + " magically welds itself onto " +
                entity.get_possessive() + " " +
                entity.get_bodypart_name(BodyPart::Hand) + "!";
      the_message_log.add(message);
    }
  }

  return subclass_result;
}

bool Thing::perform_action_fired_by(Entity& entity, Direction direction)
{
  return _perform_action_fired_by(entity, direction);
}

char const* Thing::get_thing_type() const
{
  return typeid(*this).name();
}

bool Thing::is_liquid_carrier() const
{
  return false;
}

bool Thing::is_flammable() const
{
  return false;
}

bool Thing::is_corrodible() const
{
  return false;
}

bool Thing::is_shatterable() const
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

// *** PRIVATE METHODS ********************************************************

bool Thing::_perform_action_activated_by(Entity& entity)
{
  return false;
}

void Thing::_perform_action_collided_with(Thing& thing)
{
}

bool Thing::_perform_action_drank_by(Entity& entity)
{
  return false;
}

bool Thing::_perform_action_dropped_by(Entity& entity)
{
  return true;
}

bool Thing::_perform_action_eaten_by(Entity& entity)
{
  return false;
}

bool Thing::_perform_action_picked_up_by(Entity& entity)
{
  return true;
}

bool Thing::_perform_action_put_into(Thing& container)
{
  return true;
}

bool Thing::_perform_action_take_out()
{
  return true;
}

ActionResult Thing::_perform_action_read_by(Entity& entity)
{
  return ActionResult::Failure;
}

void Thing::_perform_action_attack_hits(Entity& entity)
{
}

bool Thing::_perform_action_thrown_by(Entity& thing, Direction direction)
{
  return true;
}

bool Thing::_perform_action_deequipped_by(Entity& entity,
                                          WearLocation& location)
{
  return true;
}

bool Thing::_perform_action_equipped_by(Entity& entity, WearLocation& location)
{
  return false;
}

bool Thing::_perform_action_unwielded_by(Entity& entity)
{
  return true;
}

bool Thing::_perform_action_wielded_by(Entity& entity)
{
  return true;
}

bool Thing::_perform_action_fired_by(Entity& entity, Direction direction)
{
  return false;
}
