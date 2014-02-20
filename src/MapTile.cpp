#include "MapTile.h"

#include <boost/random/uniform_int_distribution.hpp>
#include <string>

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "Map.h"
#include "MathUtils.h"
#include "ThingFactory.h"
#include "TileSheet.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapTile::Impl
{
    MapId map_id;
    sf::Vector2i coords;

    /// Tile's light level.
    /// Levels for the various color channels are interpreted as such:
    /// 0 <= value <= 128: result = (original * (value / 128))
    /// 128 < value <= 255: result = max(original + (value - 128), 255)
    /// The alpha channel is ignored.
    sf::Color ambient_light_color;

    /// A map of (thing_id, LightInfluence) pairs, representing the amount of
    /// light that each thing_id is contributing to this map tile.
    /// Levels for the various color channels are interpreted as such:
    /// 0 <= value <= 128: result = (original * (value / 128))
    /// 128 < value <= 255: result = max(original + (value - 128), 255)
    /// The alpha channel is ignored.
    std::map<ThingId, LightInfluence> lights;

    MapTileType type;
    //unsigned int variant;
};

MapTile::MapTile(MapId mapId, int x, int y)
  : Container(), impl(new Impl())
{
  //uniform_int_dist vDist(0, 3);

  impl->type = MapTileType::FloorStone;
  //impl->variant = vDist(the_RNG);
  impl->map_id = mapId;
  impl->coords.x = x;
  impl->coords.y = y;
  impl->ambient_light_color = sf::Color(192, 192, 192, 255);
}

MapTile::~MapTile()
{
}

std::string MapTile::_get_description() const
{
  return getMapTileTypeDescription(impl->type);
}

sf::Vector2u MapTile::get_tile_sheet_coords(int frame) const
{
  sf::Vector2u result = getMapTileTypeTileSheetCoords(impl->type);
  //result.x += impl->variant;

  return result;
}

void MapTile::set_type(MapTileType tileType)
{
  impl->type = tileType;
}

MapTileType MapTile::get_type() const
{
  return impl->type;
}

bool MapTile::is_empty_space() const
{
  return getMapTileTypePassable(impl->type);
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal Entity can move through solid matter.
bool MapTile::can_be_traversed_by(Entity& entity) const
{
  return is_empty_space();
}

void MapTile::set_coords(int x, int y)
{
  impl->coords.x = x;
  impl->coords.y = y;
}

sf::Vector2i const& MapTile::get_coords() const
{
  return impl->coords;
}

MapId MapTile::get_map_id() const
{
  return impl->map_id;
}

void MapTile::set_ambient_light_level(sf::Color level)
{
  impl->ambient_light_color = level;
}

void MapTile::be_lit_by(LightSource& light)
{
  MF.get(get_map_id()).add_light(light);
}


void MapTile::clear_light_influences()
{
  impl->lights.clear();
}

void MapTile::add_light_influence(ThingId source, LightInfluence influence)
{
  impl->lights[source] = influence;
}

sf::Color MapTile::get_light_level() const
{
  sf::Color color = impl->ambient_light_color;

  for (std::map<ThingId, LightInfluence>::const_iterator iter = impl->lights.begin();
       iter != impl->lights.end();
       ++iter)
  {
    sf::Vector2i const& source_coords = (*iter).second.coords;
    int dist_squared = calc_vis_distance(get_coords().x, get_coords().y,
                                         source_coords.x, source_coords.y);

    sf::Color light_color = (*iter).second.color;
    int light_intensity = (*iter).second.intensity;

    bool light_is_visible = TF.get_player().can_see(source_coords.x, source_coords.y);

    sf::Color addColor;

    // LightIntensity is the distance at which the calculated light would be
    // zero.
    //

    // If this tile is opaque, make sure we can see the light source.
    // This is a crude but effective way of making sure light does not
    // "bleed through" wall tiles.
    if (!is_opaque() || light_is_visible)
    {

      // Although called "percentage" this actually ranges from 0 to 65536.
      int dist_percentage;

      if (light_intensity == 0)
      {
        dist_percentage = 65536;
      }
      else
      {
        dist_percentage = (dist_squared * 65536) / light_intensity;
      }

      int light_factor = (65536 - dist_percentage);

      addColor.r = (static_cast<int>(light_color.r) * light_factor) / 65536;
      addColor.g = (static_cast<int>(light_color.g) * light_factor) / 65536;
      addColor.b = (static_cast<int>(light_color.b) * light_factor) / 65536;
      addColor.a = 255;

      color.r = saturation_add(color.r, addColor.r);
      color.g = saturation_add(color.g, addColor.g);
      color.b = saturation_add(color.b, addColor.b);
      color.a = saturation_add(color.a, addColor.a);
    }
  }

  return color;
}

bool MapTile::is_opaque() const
{
  /// @todo Check the tile's inventory to see if there's anything huge enough
  ///       to block the view of stuff behind it.
  return getMapTileTypeOpaque(get_type());
}

void MapTile::draw_highlight(sf::RenderTarget& target,
                            sf::Vector2f location,
                            sf::Color fgColor,
                            sf::Color bgColor,
                            int frame)
{
  float ts2(static_cast<float>(Settings.map_tile_size) * 0.5);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);

  sf::RectangleShape box_shape;
  sf::Vector2f box_position;
  sf::Vector2f box_size(Settings.map_tile_size, Settings.map_tile_size);
  sf::Vector2f box_half_size(box_size.x / 2, box_size.y / 2);
  box_position.x = (location.x - box_half_size.x);
  box_position.y = (location.y - box_half_size.y);
  box_shape.setPosition(box_position);
  box_shape.setSize(box_size);
  box_shape.setOutlineColor(fgColor);
  box_shape.setOutlineThickness(Settings.tile_highlight_border_width);
  box_shape.setFillColor(bgColor);

  target.draw(box_shape);
}

void MapTile::add_walls_to(sf::VertexArray& vertices,
                           bool use_lighting,
                           bool nw_is_empty, bool n_is_empty,
                           bool ne_is_empty, bool e_is_empty,
                           bool se_is_empty, bool s_is_empty,
                           bool sw_is_empty, bool w_is_empty)
{
  // Tile color.
  sf::Color tile_color;

  // Full tile size.
  float ts(static_cast<float>(Settings.map_tile_size));

  // Half of the tile size.
  float ts2(static_cast<float>(Settings.map_tile_size) * 0.5);


  // Wall size (configurable).
  float ws(static_cast<float>(Settings.map_tile_size) * 0.4);

  // Tile vertices.
  sf::Vector2f location(impl->coords.x * ts,
                        impl->coords.y * ts);
  sf::Vector2f vTileNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vTileNE(location.x + ts2, location.y - ts2);
  sf::Vector2f vTileSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vTileSE(location.x + ts2, location.y + ts2);
  sf::Vector2u tile_coords = this->get_tile_sheet_coords(0);

  if (use_lighting)
  {
    tile_color = get_light_level();
  }
  else
  {
    tile_color = sf::Color::White;
  }

  // NORTH WALL
  if (n_is_empty)
  {
    sf::Vector2f vSW(vTileNW.x, vTileNW.y + ws);
    if (w_is_empty)
    {
      vSW.x += ws;
    }
    else if (!nw_is_empty)
    {
      vSW.x -= ws;
    }
    sf::Vector2f vSE(vTileNE.x, vTileNE.y + ws);
    if (e_is_empty)
    {
      vSE.x -= ws;
    }
    else if (!ne_is_empty)
    {
      vSE.x += ws;
    }

    // DEBUG
    //tile_color = sf::Color::Red;

    TileSheet::add_vertices(vertices,
                            tile_coords, tile_color,
                            vTileNW, vTileNE, vSW, vSE);
  }

  // EAST WALL
  if (e_is_empty)
  {
    sf::Vector2f vNW(vTileNE.x - ws, vTileNE.y);
    if (n_is_empty)
    {
      vNW.y += ws;
    }
    else if (!ne_is_empty)
    {
      vNW.y -= ws;
    }
    sf::Vector2f vSW(vTileSE.x - ws, vTileSE.y);
    if (s_is_empty)
    {
      vSW.y -= ws;
    }
    else if (!se_is_empty)
    {
      vSW.y += ws;
    }

    // DEBUG
    //tile_color = sf::Color::Yellow;

    TileSheet::add_vertices(vertices,
                            tile_coords, tile_color,
                            vNW, vTileNE, vSW, vTileSE);
  }

  // SOUTH WALL
  if (s_is_empty)
  {
    sf::Vector2f vNW(vTileSW.x, vTileSW.y - ws);
    if (w_is_empty)
    {
      vNW.x += ws;
    }
    else if (!sw_is_empty)
    {
      vNW.x -= ws;
    }
    sf::Vector2f vNE(vTileSE.x, vTileSE.y - ws);
    if (e_is_empty)
    {
      vNE.x -= ws;
    }
    else if (!se_is_empty)
    {
      vNE.x += ws;
    }

    // DEBUG
    //tile_color = sf::Color::Green;

    TileSheet::add_vertices(vertices,
                            tile_coords, tile_color,
                            vNW, vNE, vTileSW, vTileSE);
  }

  // WEST WALL
  if (w_is_empty)
  {
    sf::Vector2f vNE(vTileNW.x + ws, vTileNW.y);
    if (n_is_empty)
    {
      vNE.y += ws;
    }
    else if (!nw_is_empty)
    {
      vNE.y -= ws;
    }
    sf::Vector2f vSE(vTileSW.x + ws, vTileSW.y);
    if (s_is_empty)
    {
      vSE.y -= ws;
    }
    else if (!sw_is_empty)
    {
      vSE.y += ws;
    }

    // DEBUG
    //tile_color = sf::Color::Blue;

    TileSheet::add_vertices(vertices,
                            tile_coords, tile_color,
                            vTileNW, vNE, vTileSW, vSE);
  }

  /// @todo Implement me.
}

ActionResult MapTile::can_contain(Thing& thing) const
{
  /// @todo Implement can_contain.  Right now a MapTile can contain any Thing.
  return ActionResult::Success;
}

bool MapTile::readable_by(Entity const& entity) const
{
  return true;
}

ActionResult MapTile::_perform_action_read_by(Entity& entity)
{
  /// @todo Implement perform_action_read_by (e.g. writing/engraving on walls).
  return ActionResult::Failure;
}

sf::Vector2f MapTile::get_pixel_coords(int x, int y)
{
  return sf::Vector2f(x * Settings.map_tile_size,
                      y * Settings.map_tile_size);
}

sf::Vector2f MapTile::get_pixel_coords(sf::Vector2i tile)
{
  return get_pixel_coords(tile.x, tile.y);
}
