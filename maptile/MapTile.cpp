#include "stdafx.h"

#include "maptile/MapTile.h"

#include "entity/EntityPool.h"
#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "types/Color.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"

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
  return m_p_type_data->value("name", "MTUnknown");
}

void MapTile::setTileType(std::string type)
{
  m_type = type;
  m_p_type_data = &(GAME.category(type));
}

std::string MapTile::getTileType() const
{
  return m_type;
}

bool MapTile::isEmptySpace() const
{
  return m_p_type_data->value("passable", false);
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

void MapTile::setAmbientLightLevel(Color level)
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

    Color const& light_color = influence.color;
    int light_intensity = influence.intensity;

    Color addColor{ 0, 0, 0, 255 };

    float dist_factor;

    if (light_intensity == 0)
    {
      dist_factor = 1.0f;
    }
    else
    {
      dist_factor = dist_squared / static_cast<float>(light_intensity);
    }

    std::vector<Direction> const directions
    { 
      Direction::Self, 
      Direction::North, 
      Direction::East, 
      Direction::South, 
      Direction::West 
    };

    for (Direction d : directions)
    {
      //if (!isOpaque() || (d != Direction::Self))
      {
        float light_factor = (1.0f - dist_factor);
        float wall_factor = Direction::calculate_light_factor(influence.coords, getCoords(), d);
        float factor = wall_factor * light_factor;

        float newR = static_cast<float>(light_color.r()) * factor;
        float newG = static_cast<float>(light_color.g()) * factor;
        float newB = static_cast<float>(light_color.b()) * factor;

        addColor.setR(newR);
        addColor.setG(newG);
        addColor.setB(newB);

        unsigned int index = d.get_map_index();
        auto prevColor = m_calculated_light_colors[index];

        m_calculated_light_colors[index] = prevColor + addColor;
      }
    }
  }
}

Color MapTile::getLightLevel() const
{
  return getWallLightLevel(Direction::Self);
}

Color MapTile::getWallLightLevel(Direction direction) const
{
  if (m_calculated_light_colors.count(direction.get_map_index()) == 0)
  {
    return m_ambient_light_color;
  }
  else
  {
    return m_ambient_light_color + m_calculated_light_colors.at(direction.get_map_index());
  }
}

Color MapTile::getOpacity() const
{
  return m_p_type_data->value("opacity", Color::White);
}

bool MapTile::isOpaque() const
{
  /// @todo Check the tile's inventory to see if there's anything huge enough
  ///       to block the view of stuff behind it.
  return (getOpacity() == Color::White);
}

RealVec2 MapTile::getPixelCoords(IntVec2 tile)
{
  auto& config = Service<IConfigSettings>::get();
  float map_tile_size = config.get("map-tile-size");

  return RealVec2(static_cast<float>(tile.x) * map_tile_size,
                  static_cast<float>(tile.y) * map_tile_size);
}

// === PROTECTED METHODS ======================================================

MapTile::MapTile(IntVec2 coords, json& data, MapId map_id)
  :
  m_map_id{ map_id },
  m_coords{ coords },
  m_p_type_data{ &data },
  m_ambient_light_color{ Color(192, 192, 192, 255) }
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

json const& MapTile::getTypeData() const
{
  return *m_p_type_data;
}
