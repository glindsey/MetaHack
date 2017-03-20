#include "stdafx.h"

#include "maptile/MapTile.h"

#include "game/App.h"
#include "game/GameState.h"
#include "services/IConfigSettings.h"
#include "map/Map.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"
#include "Service.h"
#include "entity/EntityPool.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

bool MapTile::initialized = false;

MapTile::~MapTile()
{}

EntityId MapTile::get_tile_contents() const
{
  return m_tile_contents;
}

std::string MapTile::get_display_name() const
{
  return m_p_metadata->get_intrinsic("name").as<std::string>();
}

void MapTile::set_tile_type(std::string type)
{
  m_p_metadata = &(m_p_metadata->get_metadata_collection().get(type));
}

std::string MapTile::get_tile_type() const
{
  return m_p_metadata->get_type();
}

bool MapTile::is_empty_space() const
{
  return m_p_metadata->get_intrinsic("passable").as<bool>();
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal DynamicEntity can move through solid matter.
bool MapTile::can_be_traversed_by(EntityId entity) const
{
  return is_empty_space();
}

void MapTile::set_coords(int x, int y)
{
  m_coords.x = x;
  m_coords.y = y;
}

void MapTile::set_coords(IntVec2 coords)
{
  m_coords = coords;
}

IntVec2 const& MapTile::get_coords() const
{
  return m_coords;
}

MapId MapTile::get_map_id() const
{
  return m_map_id;
}

void MapTile::set_ambient_light_level(sf::Color level)
{
  m_ambient_light_color = level;
}

void MapTile::be_lit_by(EntityId light)
{
  GAME.get_maps().get(get_map_id()).add_light(light);
}

void MapTile::clear_light_influences()
{
  m_lights.clear();
  m_calculated_light_colors.clear();
}

void MapTile::add_light_influence(EntityId source,
                                  LightInfluence influence)
{
  if (m_lights.count(source) == 0)
  {
    m_lights[source] = influence;

    float dist_squared = static_cast<float>(calc_vis_distance(get_coords().x, get_coords().y, influence.coords.x, influence.coords.y));

    sf::Color light_color = influence.color;
    int light_intensity = influence.intensity;

    sf::Color addColor;

    float dist_factor;

    if (light_intensity == 0)
    {
      dist_factor = 1.0f;
    }
    else
    {
      dist_factor = dist_squared / static_cast<float>(light_intensity);
    }

    std::vector<Direction> const directions{ Direction::Self, Direction::North, Direction::East, Direction::South, Direction::West };

    for (Direction d : directions)
    {
      //if (!is_opaque() || (d != Direction::Self))
      {
        float light_factor = (1.0f - dist_factor);
        float wall_factor = Direction::calculate_light_factor(influence.coords, get_coords(), d);

        addColor.r = static_cast<sf::Uint8>(static_cast<float>(light_color.r) * wall_factor * light_factor);
        addColor.g = static_cast<sf::Uint8>(static_cast<float>(light_color.g) * wall_factor * light_factor);
        addColor.b = static_cast<sf::Uint8>(static_cast<float>(light_color.b) * wall_factor * light_factor);
        addColor.a = 255;

        unsigned int index = d.get_map_index();
        m_calculated_light_colors[index] = saturation_add(m_calculated_light_colors[index], addColor);
      }
    }
  }
}

sf::Color MapTile::get_light_level() const
{
  if (m_calculated_light_colors.count(Direction::Self.get_map_index()) == 0)
  {
    return m_ambient_light_color; // sf::Color::Black;
  }
  else
  {
    return saturation_add(m_ambient_light_color, m_calculated_light_colors.at(Direction::Self.get_map_index()));
  }
}

sf::Color MapTile::get_wall_light_level(Direction direction) const
{
  if (m_calculated_light_colors.count(direction.get_map_index()) == 0)
  {
    return m_ambient_light_color; // sf::Color::Black;
  }
  else
  {
    return saturation_add(m_ambient_light_color, m_calculated_light_colors.at(direction.get_map_index()));
  }
}

sf::Color MapTile::get_opacity() const
{
  return sf::Color(m_p_metadata->get_intrinsic("opacity_red").as<int>(),
                   m_p_metadata->get_intrinsic("opacity_green").as<int>(),
                   m_p_metadata->get_intrinsic("opacity_blue").as<int>(),
                   255);
}

bool MapTile::is_opaque() const
{
  /// @todo Check the tile's inventory to see if there's anything huge enough
  ///       to block the view of stuff behind it.
  return
    (m_p_metadata->get_intrinsic("opacity_red").as<int>() >= 255) &&
    (m_p_metadata->get_intrinsic("opacity_green").as<int>() >= 255) &&
    (m_p_metadata->get_intrinsic("opacity_blue").as<int>() >= 255);
}

RealVec2 MapTile::get_pixel_coords(int x, int y)
{
  auto& config = Service<IConfigSettings>::get();
  auto map_tile_size = config.get("map_tile_size").as<float>();

  return RealVec2(static_cast<float>(x) * map_tile_size, 
               static_cast<float>(y) * map_tile_size);
}

RealVec2 MapTile::get_pixel_coords(IntVec2 tile)
{
  return get_pixel_coords(tile.x, tile.y);
}

// === PROTECTED METHODS ======================================================

MapTile::MapTile(IntVec2 coords, Metadata& metadata, MapId map_id)
  :
  m_map_id{ map_id },
  m_coords{ coords },
  m_p_metadata{ &metadata },
  m_ambient_light_color{ sf::Color(192, 192, 192, 255) }
{
  if (!initialized)
  {
    // TODO: any static class initialization that must be performed
    initialized = true;
  }

  // Tile contents entity is created here, or else the pImpl would need the
  // "this" pointer passed in.
  /// @todo The type of this floor should eventually be specified as
  ///       part of the constructor.
  m_tile_contents = GAME.get_entities().create_tile_contents(this);
}

MapTile const& MapTile::get_adjacent_tile(Direction direction) const
{
  IntVec2 coords = get_coords();
  Map const& map = GAME.get_maps().get(get_map_id());
  MapTile const& tile = *this;

  IntVec2 adjacent_coords = coords + (IntVec2)direction;
  return map.get_tile(adjacent_coords);
}

Metadata const & MapTile::get_metadata() const
{
  return *m_p_metadata;
}
