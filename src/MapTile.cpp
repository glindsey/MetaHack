#include "MapTile.h"

#include <boost/log/trivial.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <string>

#include "App.h"
#include "ConfigSettings.h"
#include "Map.h"
#include "MapTileMetadata.h"
#include "MathUtils.h"
#include "ThingManager.h"
#include "TileSheet.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

bool MapTile::initialized = false;

struct MapTile::Impl
{
  Impl() {}
  ~Impl()
  {
    lights.clear();
  }

  MapId map_id;
  sf::Vector2i coords;

  /// Reference to the Thing that represents this tile's floor.
  ThingRef floor;

  /// Tile's light level.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  sf::Color ambient_light_color;

  /// A map of LightInfluences, representing the amount of light that
  /// each thing is contributing to this map tile.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  std::unordered_map<ThingRef, LightInfluence> lights;

  std::string type;
  MapTileMetadata* pMetadata;
};

MapTile::MapTile(sf::Vector2i coords, MapId mapId)
  : pImpl(NEW Impl())
{
  if (!initialized)
  {
    // TODO: initialization
  }

  //uniform_int_dist vDist(0, 3);

  pImpl->floor = TM.create_floor(this);
  this->set_type("FloorStone");
  //pImpl->variant = vDist(the_RNG);
  pImpl->map_id = mapId;
  pImpl->coords = coords;
  pImpl->ambient_light_color = sf::Color(192, 192, 192, 255);
}

MapTile::~MapTile()
{
  pImpl.reset();
}

ThingRef MapTile::get_floor() const
{
  return pImpl->floor;
}

std::string MapTile::get_display_name() const
{
  return pImpl->pMetadata->get_display_name();
}

sf::Vector2u MapTile::get_tile_sheet_coords(int frame) const
{
  /// @todo Deal with selecting one of the other tiles.
  sf::Vector2u coords = pImpl->pMetadata->get_tile_coords();
  return coords;
}

void MapTile::add_vertices_to(sf::VertexArray& vertices,
                              bool use_lighting,
                              int frame)
{
  sf::Vertex new_vertex;
  float ts = static_cast<float>(Settings.map_tile_size);
  float ts2 = ts * 0.5;

  sf::Vector2i const& coords = get_coords();

  sf::Color light { sf::Color::White };

  if (use_lighting)
  {
    light = get_light_level();
    if (is_opaque())
    {
      light.r *= 0.5;
      light.g *= 0.5;
      light.b *= 0.5;
    }
  }
  else
  {
    light = sf::Color::White;
  }

  sf::Vector2f location(coords.x * ts, coords.y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);
  sf::Vector2u tile_coords = this->get_tile_sheet_coords(frame);

  TileSheet::add_quad(vertices,
                          tile_coords, light,
                          vNW, vNE, vSW, vSE);
}

void MapTile::draw_to(sf::RenderTexture& target,
                      sf::Vector2f target_coords,
                      unsigned int target_size,
                      bool use_lighting,
                      int frame)
{
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
    thing_color = get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  rectangle.setPosition(target_coords);
  rectangle.setSize(sf::Vector2f(target_size, target_size));
  rectangle.setTexture(&(TS.getTexture()));
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

  target.draw(rectangle);
}

void MapTile::set_type(std::string type)
{
  pImpl->type = type;
  pImpl->pMetadata = MapTileMetadata::get(type);
}

std::string MapTile::get_type() const
{
  return pImpl->type;
}

bool MapTile::is_empty_space() const
{
  return pImpl->pMetadata->get_flag("passable");
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal Entity can move through solid matter.
bool MapTile::can_be_traversed_by(ThingRef thing) const
{
  return is_empty_space();
}

void MapTile::set_coords(int x, int y)
{
  pImpl->coords.x = x;
  pImpl->coords.y = y;
}

sf::Vector2i const& MapTile::get_coords() const
{
  return pImpl->coords;
}

MapId MapTile::get_map_id() const
{
  return pImpl->map_id;
}

void MapTile::set_ambient_light_level(sf::Color level)
{
  pImpl->ambient_light_color = level;
}

void MapTile::be_lit_by(ThingRef light)
{
  MF.get(get_map_id()).add_light(light);
}

void MapTile::clear_light_influences()
{
  pImpl->lights.clear();
}

void MapTile::add_light_influence(ThingRef source,
                                  LightInfluence influence)
{

  pImpl->lights[source] = influence;
}

sf::Color MapTile::get_light_level() const
{
  sf::Color color = pImpl->ambient_light_color;

  ThingRef player = TM.get_player();

  if (player != TM.get_mu())
  {
    for (auto iter = std::begin(pImpl->lights);
      iter != std::end(pImpl->lights);
      ++iter)
    {
      sf::Vector2i const& source_coords = iter->second.coords;
      float dist_squared = calc_vis_distance(get_coords().x, get_coords().y,
        source_coords.x, source_coords.y);

      sf::Color light_color = iter->second.color;
      float light_intensity = iter->second.intensity;

      bool light_is_visible = player->can_see(source_coords.x, source_coords.y);

      sf::Color addColor;

      // LightIntensity is the distance at which the calculated light would be
      // zero.
      //

      // If this tile is opaque, make sure we can see the light source.
      // This is a crude but effective way of making sure light does not
      // "bleed through" wall tiles.
      if (!is_opaque() || light_is_visible)
      {
        float dist_factor;

        if (light_intensity == 0)
        {
          dist_factor = 1.0f;
        }
        else
        {
          dist_factor = dist_squared / light_intensity;
        }

        float light_factor = (1.0f - dist_factor);

        addColor.r = (light_color.r * light_factor);
        addColor.g = (light_color.g * light_factor);
        addColor.b = (light_color.b * light_factor);
        addColor.a = 255;

        color.r = saturation_add(color.r, addColor.r);
        color.g = saturation_add(color.g, addColor.g);
        color.b = saturation_add(color.b, addColor.b);
        color.a = saturation_add(color.a, addColor.a);
      }
    }
  }

  return color;
}

sf::Color MapTile::get_wall_light_level(Direction direction) const
{
  sf::Color color = pImpl->ambient_light_color;

  ThingRef player = TM.get_player();

  if (player != TM.get_mu())
  {
    for (auto iter = std::begin(pImpl->lights);
      iter != std::end(pImpl->lights);
      ++iter)
    {
      sf::Vector2i const& source_coords = iter->second.coords;
      float dist_squared = calc_vis_distance(get_coords().x, get_coords().y,
        source_coords.x, source_coords.y);

      sf::Color light_color = iter->second.color;
      float light_intensity = iter->second.intensity;

      bool light_is_visible = player->can_see(source_coords.x, source_coords.y);

      sf::Color addColor;

      // LightIntensity is the distance at which the calculated light would be
      // zero.
      //

      float dist_factor;

      if (light_intensity == 0)
      {
        dist_factor = 1.0f;
      }
      else
      {
        dist_factor = dist_squared / light_intensity;
      }

      float light_factor = (1.0f - dist_factor);

      float wall_factor = calculate_light_factor(source_coords, get_coords(), direction);

      addColor.r = (light_color.r * wall_factor * light_factor);
      addColor.g = (light_color.g * wall_factor * light_factor);
      addColor.b = (light_color.b * wall_factor * light_factor);
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
  return pImpl->pMetadata->get_flag("opaque");
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
  // Checks to see N/S/E/W walls.
  bool player_sees_n_wall { false };
  bool player_sees_s_wall { false };
  bool player_sees_e_wall { false };
  bool player_sees_w_wall { false };

  // Player.
  ThingRef player = TM.get_player();

  if (player != TM.get_mu())
  {
    auto player_tile = player->get_maptile();
    if (player_tile != nullptr)
    {
      player_sees_n_wall = (player_tile->get_coords().y <= get_coords().y);
      player_sees_s_wall = (player_tile->get_coords().y >= get_coords().y);
      player_sees_e_wall = (player_tile->get_coords().x >= get_coords().x);
      player_sees_w_wall = (player_tile->get_coords().x <= get_coords().x);
    }
  }

  // Tile color.
  sf::Color tile_color    { sf::Color::White };

  // Wall colors.
  sf::Color wall_color_n  { sf::Color::White };
  sf::Color wall_color_e  { sf::Color::White };
  sf::Color wall_color_s  { sf::Color::White };
  sf::Color wall_color_w  { sf::Color::White };

  // Wall light factor.
  float wall_factor;

  // Full tile size.
  float ts(static_cast<float>(Settings.map_tile_size));

  // Half of the tile size.
  float ts2(static_cast<float>(Settings.map_tile_size) * 0.5);

  // Wall size (configurable).
  float ws(static_cast<float>(Settings.map_tile_size) * 0.4);

  // Tile vertices.
  sf::Vector2f location(pImpl->coords.x * ts,
                        pImpl->coords.y * ts);
  sf::Vector2f vTileNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vTileNE(location.x + ts2, location.y - ts2);
  sf::Vector2f vTileSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vTileSE(location.x + ts2, location.y + ts2);
  sf::Vector2u tile_coords = this->get_tile_sheet_coords(0);

  if (use_lighting)
  {
    tile_color = get_light_level();
    wall_color_n = get_wall_light_level(Direction::North);
    wall_color_e = get_wall_light_level(Direction::East);
    wall_color_s = get_wall_light_level(Direction::South);
    wall_color_w = get_wall_light_level(Direction::West);
  }

  // NORTH WALL
  if (n_is_empty && player_sees_n_wall)
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

    TileSheet::add_quad(vertices,
                            tile_coords, wall_color_n,
                            vTileNW, vTileNE, vSW, vSE);
  }

  // EAST WALL
  if (e_is_empty && player_sees_e_wall)
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

    TileSheet::add_quad(vertices,
                            tile_coords, wall_color_e,
                            vNW, vTileNE, vSW, vTileSE);
  }

  // SOUTH WALL
  if (s_is_empty && player_sees_s_wall)
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

    TileSheet::add_quad(vertices,
                            tile_coords, wall_color_s,
                            vNW, vNE, vTileSW, vTileSE);
  }

  // WEST WALL
  if (w_is_empty && player_sees_w_wall)
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

    TileSheet::add_quad(vertices,
                            tile_coords, wall_color_w,
                            vTileNW, vNE, vTileSW, vSE);
  }
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
