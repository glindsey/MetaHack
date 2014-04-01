#include "Thing.h"

#include <boost/lexical_cast.hpp>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include "App.h"
#include "ConfigSettings.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
#include "LightSource.h"
#include "Map.h"
#include "MapTile.h"
#include "MessageLog.h"
#include "ThingFactory.h"
#include "TileSheet.h"

struct Thing::Impl
{
  std::weak_ptr<Thing> location;
  Inventory inventory;
  int inventory_size;
  unsigned int quantity;
  Direction direction = Direction::None;  ///< Direction the thing is facing.
  Qualities qualities;
  std::string proper_name;
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

Thing::Thing(int inventory_size)
  : impl(new Impl())
{
  impl->inventory_size = inventory_size;
  impl->qualities.physical_mass = 1;
  impl->quantity = 1;
}

Thing::Thing(const Thing& original)
  : impl(new Impl())
{
  impl->inventory_size = original.get_inventory_size();
  impl->location = original.get_location();
  impl->direction = original.get_facing_direction();
  impl->quantity = original.get_quantity();
  impl->qualities = *(original.get_qualities_pointer());
}

Thing::~Thing()
{
}

std::shared_ptr<Thing> Thing::clone()
{
  return std::shared_ptr<Thing>();
}

bool Thing::is_player() const
{
  return false;
}

unsigned int Thing::get_quantity() const
{
  return impl->quantity;
}

void Thing::set_quantity(unsigned int quantity)
{
  impl->quantity = quantity;
}

std::shared_ptr<Thing> Thing::get_location() const
{
  return impl->location.lock();
}

Inventory& Thing::get_inventory()
{
  return impl->inventory;
}

int const Thing::get_inventory_size() const
{
  return impl->inventory_size;
}

void Thing::set_inventory_size(int number)
{
  /// @todo Don't allow shrinking below current inventory size, or perhaps
  ///       automatically pop things out that don't fit!
  impl->inventory_size = number;
}

bool Thing::is_inside_another_thing() const
{
  auto location = impl->location.lock();
  if (location == nullptr)
  {
    // Thing is a part of the MapTile such as the floor.
    return false;
  }
  auto location2 = location->get_location();
  if (location2 == nullptr)
  {
    // Thing is directly on the floor.
    return false;
  }
  return true;
}
MapTile* Thing::get_maptile() const
{
  auto location = impl->location.lock();
  if (location == nullptr)
  {
    return _get_maptile();
  }
  else
  {
    return location->get_maptile();
  }
}

MapId Thing::get_map_id() const
{
  auto location = impl->location.lock();
  if (location == nullptr)
  {
    MapTile* maptile = _get_maptile();
    if (maptile != nullptr)
    {
      return maptile->get_map_id();
    }
    else
    {
      return static_cast<MapId>(0);
    }
  }
  else
  {
    return location->get_map_id();
  }
}

void Thing::set_facing_direction(Direction d)
{
  impl->direction = d;
}

Direction Thing::get_facing_direction() const
{
  return impl->direction;
}

void Thing::set_single_mass(int mass)
{
  impl->qualities.physical_mass = mass;
}

int Thing::get_single_mass() const
{
  return impl->qualities.physical_mass;
}

void Thing::set_bound(bool bound)
{
  impl->qualities.bound = bound;
}

void Thing::set_autobinds(bool autobinds)
{
  impl->qualities.autobinds = autobinds;
}

bool Thing::is_bound() const
{
  return impl->qualities.bound;
}

bool Thing::get_autobinds() const
{
  return impl->qualities.autobinds;
}

bool Thing::is_openable() const
{
  return false;
}

bool Thing::is_lockable() const
{
  return false;
}

bool Thing::is_open() const
{
  return impl->qualities.open;
}

bool Thing::set_open(bool open)
{
  if (is_openable())
  {
    impl->qualities.open = open;
  }

  return impl->qualities.open;
}

bool Thing::is_locked() const
{
  return impl->qualities.locked;
}

bool Thing::set_locked(bool locked)
{
  if (is_lockable())
  {
    impl->qualities.locked = locked;
  }

  return impl->qualities.locked;
}

Thing::Qualities const* Thing::get_qualities_pointer() const
{
  return &(impl->qualities);
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

std::string Thing::get_proper_name() const
{
  return impl->proper_name;
}

void Thing::set_proper_name(std::string name)
{
  impl->proper_name = name;
}

std::string Thing::get_name() const
{
  std::string name;

  auto location = this->get_location();
  unsigned int quantity = this->get_quantity();

  if (location)
  {
    name = location->get_possessive() + " ";
  }

  if (quantity == 1)
  {
    name += get_description();
  }
  else if (quantity > 1)
  {
    name += boost::lexical_cast<std::string>(get_quantity()) + " " +
            get_plural();
  }

  return name;
}

std::string Thing::get_def_name() const
{
  std::string name;

  // If the Thing has a proper name, use that.
  if (get_quantity() == 1)
  {
    std::string description = get_description();
    name = "the " + description;
  }
  else
  {
    name = boost::lexical_cast<std::string>(get_quantity()) + " " +
          get_plural();
  }

  return name;
}

std::string Thing::get_indef_name() const
{
  std::string name;

  // If the Thing has a proper name, use that.
  if (get_quantity() == 1)
  {
    std::string description = get_description();
    name = getIndefArt(description) + " " + description;
  }
  else
  {
    name = boost::lexical_cast<std::string>(get_quantity()) + " " +
          get_plural();
  }

  return name;
}

std::string const& Thing::choose_verb(std::string const& verb12,
                                      std::string const& verb3) const
{
  if (this == TF.get_player().get())
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

  if (this == TF.get_player().get())
  {
    return your;
  }
  else
  {
    return get_def_name() + "'s";
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

  MapTile* root_tile = this->get_maptile();
  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  sf::Vector2i const& coords = root_tile->get_coords();

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile->get_light_level();
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

  TileSheet::add_quad(vertices,
                          tile_coords, thing_color,
                          vNW, vNE, vSW, vSE);
}

void Thing::draw_to(sf::RenderTexture& target,
                    sf::Vector2f target_coords,
                    unsigned int target_size,
                    bool use_lighting,
                    int frame)
{
  MapTile* root_tile = this->get_maptile();

  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

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
    thing_color = root_tile->get_light_level();
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

bool Thing::is_opaque() const
{
  return true;
}

void Thing::light_up_surroundings()
{
  if (get_inventory_size() != 0)
  {
    if (!is_opaque())
    {
      auto& things = get_inventory().get_things();
      for (auto& thing : things)
      {
        thing.second->light_up_surroundings();
      }
    }
  }

  _light_up_surroundings();
}

void Thing::be_lit_by(LightSource& light)
{
  _be_lit_by(light);

  if (!is_opaque())
  {
    auto location = get_location();
    if (location)
    {
      location->be_lit_by(light);
    }
  }
}

void Thing::destroy()
{
  // Try to lock our old location.
  auto old_location = impl->location.lock();

  if (get_inventory_size() != 0)
  {
    Inventory& inventory = get_inventory();
    ThingMap const& things = inventory.get_things();

    // Step through all contents of this Thing.
    for (ThingPair thing : things)
    {
      if (old_location)
      {
        // Try to move this into the Thing's location.
        bool success = thing.second->move_into(old_location);
        if (!success)
        {
          // We couldn't move it, so just destroy it.
          thing.second->destroy();
        }
      }
      else
      {
        thing.second->destroy();
      }
    }
  }

  if (old_location)
  {
    old_location->get_inventory().remove(*this);
  }
}

bool Thing::move_into(std::shared_ptr<Thing> new_location)
{
  ASSERT_CONDITION(new_location);

  if (is_movable())
  {
    if (new_location->can_contain(*this) == ActionResult::Success)
    {
      if (new_location->get_inventory().add(shared_from_this()))
      {
        // Try to lock our old location.
        auto old_location = impl->location.lock();
        if (old_location != nullptr)
        {
          old_location->get_inventory().remove(*this);
        }

        // Set the location to the new location.
        impl->location = new_location;
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
  // Process inventory.
  auto things = impl->inventory.get_things();

  for (auto iter = std::begin(things);
            iter != std::end(things);
            /* no increment */)
  {
    bool dead = iter->second->do_process();
    if (dead)
    {
      things.erase(iter++);
    }
    else
    {
      ++iter;
    }
  }

  // Process self last.
  return _do_process();
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
  if (this->is_bound())
  {
    std::string message;
    message = entity.get_name() + " cannot take off " + this->get_name() +
              "; it is magically bound to " +
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
    if (this->get_autobinds())
    {
        this->set_bound(true);
        std::string message;
        message = this->get_name() + " magically binds itself to " +
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
  if (this->is_bound())
  {
    std::string message;
    message = entity.get_name() + " cannot unwield " + this->get_name() +
              "; it is magically bound to " +
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
    if (this->get_autobinds())
    {
      this->set_bound(true);
      std::string message;
      message = this->get_name() + " magically binds itself to " +
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

bool Thing::can_merge_with(Thing const& other) const
{
  // FUCK IT, I'M USING RTTI HERE.
  // I can probably craft a solution using CRTP and double-dispatch but I will
  // almost certainly go insane figuring it out AND waste a ton of time.

  // Things with different types can't merge (obviously).
  if (typeid(other) != typeid(*this))
  {
    return false;
  }

  // Things with inventories can never merge.
  if ((impl->inventory_size != 0) ||
      (other.get_inventory_size() != 0))
  {
    return false;
  }

  // If the things have the same qualities, merge is okay.
  if (this->has_same_qualities_as(other))
  {
    return true;
  }

  return false;
}

ActionResult Thing::can_contain(Thing& thing) const
{
  if (impl->inventory_size == 0)
  {
    return ActionResult::FailureTargetNotAContainer;
  }
  else
  {
    return _can_contain(thing);
  }
}

//char const* Thing::get_thing_type() const
//{
//  return typeid(*this).name();
//}

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

void Thing::set_location(std::weak_ptr<Thing> target)
{
  impl->location = target;
}

void Thing::_light_up_surroundings()
{
  // default behavior does nothing
}

void Thing::_be_lit_by(LightSource& light)
{
  // default behavior does nothing
}

// *** PRIVATE METHODS ********************************************************

MapTile* Thing::_get_maptile() const
{
  return nullptr;
}

ActionResult Thing::_can_contain(Thing& thing) const
{
  return ActionResult::Success;
}

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

bool Thing::_do_process()
{
  return true;
}

