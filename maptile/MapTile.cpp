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

EntityId MapTile::getTileContents() const
{
  return m_tile_contents;
}

std::string MapTile::getDisplayName() const
{
  return m_p_metadata->getIntrinsic("name").as<std::string>();
}

void MapTile::setTileType(std::string type)
{
  m_p_metadata = &(m_p_metadata->getMetadataCollection().get(type));
}

std::string MapTile::getTileType() const
{
  return m_p_metadata->getType();
}

bool MapTile::isEmptySpace() const
{
  return m_p_metadata->getIntrinsic("passable").as<bool>();
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal DynamicEntity can move through solid matter.
bool MapTile::canBeTraversedBy(EntityId entity) const
{
  return isEmptySpace();
}

void MapTile::setCoords(IntVec2 coords)
{
  m_coords = coords;
}

IntVec2 const& MapTile::getCoords() const
{
  return m_coords;
}

MapId MapTile::getMapId() const
{
  return m_map_id;
}

void MapTile::setAmbientLightLevel(sf::Color level)
{
  m_ambient_light_color = level;
}

void MapTile::beLitBy(EntityId light)
{
  GAME.getMaps().get(getMapId()).addLight(light);
}

void MapTile::clearLightInfluences()
{
  m_lights.clear();
  m_calculated_light_colors.clear();
}

void MapTile::addLightInfluence(EntityId source,
                                  LightInfluence influence)
{
  if (m_lights.count(source) == 0)
  {
    m_lights[source] = influence;

    float dist_squared = static_cast<float>(calc_vis_distance(getCoords().x, getCoords().y, influence.coords.x, influence.coords.y));

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
      //if (!isOpaque() || (d != Direction::Self))
      {
        float light_factor = (1.0f - dist_factor);
        float wall_factor = Direction::calculate_light_factor(influence.coords, getCoords(), d);

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

sf::Color MapTile::getLightLevel() const
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

sf::Color MapTile::getWallLightLevel(Direction direction) const
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

sf::Color MapTile::getOpacity() const
{
  return m_p_metadata->getIntrinsic("opacity").as<Color>();
}

bool MapTile::isOpaque() const
{
  /// @todo Check the tile's inventory to see if there's anything huge enough
  ///       to block the view of stuff behind it.
  auto opacity = getOpacity();
  return opacity.r >= 255 && opacity.g >= 255 && opacity.b >= 255;
}

RealVec2 MapTile::getPixelCoords(IntVec2 tile)
{
  auto& config = Service<IConfigSettings>::get();
  auto map_tile_size = config.get("map_tile_size").as<float>();

  return RealVec2(static_cast<float>(tile.x) * map_tile_size,
                  static_cast<float>(tile.y) * map_tile_size);
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
  m_tile_contents = GAME.getEntities().createTileContents(this);
}

MapTile const& MapTile::getAdjacentTile(Direction direction) const
{
  IntVec2 coords = getCoords();
  Map const& map = GAME.getMaps().get(getMapId());
  MapTile const& tile = *this;

  IntVec2 adjacent_coords = coords + (IntVec2)direction;
  return map.getTile(adjacent_coords);
}

Metadata const & MapTile::getMetadata() const
{
  return *m_p_metadata;
}
