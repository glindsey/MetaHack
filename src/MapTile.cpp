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

MapTile::~MapTile()
{}

ThingRef MapTile::get_floor() const
{
  return m_floor;
}

std::string MapTile::get_display_name() const
{
  return m_pMetadata->get_intrinsic<std::string>("name");
}

sf::Vector2u MapTile::get_tile_sheet_coords(int frame) const
{
  /// @todo Deal with selecting one of the other tiles.
  sf::Vector2u coords = m_pMetadata->get_tile_coords();
  return coords;
}

void MapTile::add_vertices_to(sf::VertexArray& vertices,
                              bool use_lighting,
                              int frame)
{
  sf::Vertex new_vertex;
  float ts = Settings.get<float>("map_tile_size");
  float half_ts = ts * 0.5f;

  sf::Vector2i const& coords = get_coords();
  MapTile const& tileN = get_adjacent_tile(Direction::North);
  MapTile const& tileNE = get_adjacent_tile(Direction::Northeast);
  MapTile const& tileE = get_adjacent_tile(Direction::East);
  MapTile const& tileSE = get_adjacent_tile(Direction::Southeast);
  MapTile const& tileS = get_adjacent_tile(Direction::South);
  MapTile const& tileSW = get_adjacent_tile(Direction::Southwest);
  MapTile const& tileW = get_adjacent_tile(Direction::West);
  MapTile const& tileNW = get_adjacent_tile(Direction::Northwest);

  sf::Color light{ sf::Color::White };
  sf::Color lightN{ sf::Color::White };
  sf::Color lightNE{ sf::Color::White };
  sf::Color lightE{ sf::Color::White };
  sf::Color lightSE{ sf::Color::White };
  sf::Color lightS{ sf::Color::White };
  sf::Color lightSW{ sf::Color::White };
  sf::Color lightW{ sf::Color::White };
  sf::Color lightNW{ sf::Color::White };

  if (use_lighting)
  {
    sf::Color colorN{ tileN.get_light_level() };
    sf::Color colorNE{ tileNE.get_light_level() };
    sf::Color colorE{ tileE.get_light_level() };
    sf::Color colorSE{ tileSE.get_light_level() };
    sf::Color colorS{ tileS.get_light_level() };
    sf::Color colorSW{ tileSW.get_light_level() };
    sf::Color colorW{ tileW.get_light_level() };
    sf::Color colorNW{ tileNW.get_light_level() };

    light = get_light_level();
    lightN = average(light, colorN);
    lightNE = average(light, colorN, colorNE, colorE);
    lightE = average(light, colorE);
    lightSE = average(light, colorE, colorSE, colorS);
    lightS = average(light, colorS);
    lightSW = average(light, colorS, colorSW, colorW);
    lightW = average(light, colorW);
    lightNW = average(light, colorW, colorNW, colorN);
  }

  sf::Vector2f location{ coords.x * ts, coords.y * ts };
  sf::Vector2f vN{ location.x, location.y - half_ts };
  sf::Vector2f vNE{ location.x + half_ts, location.y - half_ts };
  sf::Vector2f vE{ location.x + half_ts, location.y };
  sf::Vector2f vSE{ location.x + half_ts, location.y + half_ts };
  sf::Vector2f vS{ location.x, location.y + half_ts };
  sf::Vector2f vSW{ location.x - half_ts, location.y + half_ts };
  sf::Vector2f vW{ location.x - half_ts, location.y };
  sf::Vector2f vNW{ location.x - half_ts, location.y - half_ts };

  sf::Vector2u tile_coords = this->get_tile_sheet_coords(frame);

  // Northwest quadrant.
  TileSheet::add_gradient_quad(vertices, tile_coords, lightNW, vNW, lightN, vN, light, location, lightW, vW);

  // Northeast quadrant.
  TileSheet::add_gradient_quad(vertices, tile_coords, lightN, vN, lightNE, vNE, lightE, vE, light, location);

  // Southeast quadrant.
  TileSheet::add_gradient_quad(vertices, tile_coords, light, location, lightE, vE, lightSE, vSE, lightS, vS);

  // Southwest quadrant.
  TileSheet::add_gradient_quad(vertices, tile_coords, lightW, vW, light, location, lightS, vS, lightSW, vSW);
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
    target_size = Settings.get<unsigned int>("map_tile_size");
  }

  auto tile_size = Settings.get<unsigned int>("map_tile_size");

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
  rectangle.setSize(sf::Vector2f(static_cast<float>(target_size), static_cast<float>(target_size)));
  rectangle.setTexture(&(TS.getTexture()));
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

  target.draw(rectangle);
}

void MapTile::set_type(std::string type)
{
  m_pMetadata = &(m_pMetadata->get_collection().get(type));
}

std::string MapTile::get_type() const
{
  return m_pMetadata->get_type();
}

bool MapTile::is_empty_space() const
{
  return m_pMetadata->get_intrinsic<bool>("passable");
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal Entity can move through solid matter.
bool MapTile::can_be_traversed_by(ThingRef thing) const
{
  return is_empty_space();
}

void MapTile::set_coords(int x, int y)
{
  m_coords.x = x;
  m_coords.y = y;
}

sf::Vector2i const& MapTile::get_coords() const
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

void MapTile::be_lit_by(ThingRef light)
{
  MF.get(get_map_id()).add_light(light);
}

void MapTile::clear_light_influences()
{
  m_lights.clear();
}

void MapTile::add_light_influence(ThingRef source,
                                  LightInfluence influence)
{
  m_lights[source] = influence;
}

sf::Color MapTile::get_light_level() const
{
  sf::Color color = m_ambient_light_color;

  ThingRef player = TM.get_player();

  if (player != TM.get_mu())
  {
    for (auto iter = std::begin(m_lights);
      iter != std::end(m_lights);
      ++iter)
    {
      sf::Vector2i const& source_coords = iter->second.coords;
      float dist_squared = static_cast<float>(calc_vis_distance(get_coords().x, get_coords().y, source_coords.x, source_coords.y));

      sf::Color light_color = iter->second.color;
      int light_intensity = iter->second.intensity;

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
          dist_factor = dist_squared / static_cast<float>(light_intensity);
        }

        float light_factor = (1.0f - dist_factor);

        addColor.r = static_cast<sf::Uint8>(light_color.r * light_factor);
        addColor.g = static_cast<sf::Uint8>(light_color.g * light_factor);
        addColor.b = static_cast<sf::Uint8>(light_color.b * light_factor);
        addColor.a = 255;

        color.r = saturation_add(color.r, addColor.r);
        color.g = saturation_add(color.g, addColor.g);
        color.b = saturation_add(color.b, addColor.b);
        color.a = saturation_add(color.a, addColor.a);
      }
    }
  }

  // If the tile is opaque, cut the light level in half.
  if (is_opaque())
  {
    color.r >>= 1;
    color.g >>= 1;
    color.b >>= 1;
  }

  return color;
}

sf::Color MapTile::get_wall_light_level(Direction direction) const
{
  sf::Color color = m_ambient_light_color;

  ThingRef player = TM.get_player();

  if (player != TM.get_mu())
  {
    for (auto iter = std::begin(m_lights);
      iter != std::end(m_lights);
      ++iter)
    {
      sf::Vector2i const& source_coords = iter->second.coords;
      float dist_squared = static_cast<float>(calc_vis_distance(get_coords().x, get_coords().y, source_coords.x, source_coords.y));

      sf::Color light_color = iter->second.color;
      int light_intensity = iter->second.intensity;

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
        dist_factor = dist_squared / static_cast<float>(light_intensity);
      }

      float light_factor = (1.0f - dist_factor);

      float wall_factor = calculate_light_factor(source_coords, get_coords(), direction);

      addColor.r = static_cast<sf::Uint8>(light_color.r * wall_factor * light_factor);
      addColor.g = static_cast<sf::Uint8>(light_color.g * wall_factor * light_factor);
      addColor.b = static_cast<sf::Uint8>(light_color.b * wall_factor * light_factor);
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
  bool return_value = m_pMetadata->get_intrinsic<bool>("opaque");
  return return_value;
}

void MapTile::draw_highlight(sf::RenderTarget& target,
                            sf::Vector2f location,
                            sf::Color fgColor,
                            sf::Color bgColor,
                            int frame)
{
  float half_ts(Settings.get<float>("map_tile_size") * 0.5f);
  sf::Vector2f vSW(location.x - half_ts, location.y + half_ts);
  sf::Vector2f vSE(location.x + half_ts, location.y + half_ts);
  sf::Vector2f vNW(location.x - half_ts, location.y - half_ts);
  sf::Vector2f vNE(location.x + half_ts, location.y - half_ts);

  sf::RectangleShape box_shape;
  sf::Vector2f box_position;
  sf::Vector2f box_size(Settings.get<float>("map_tile_size"), Settings.get<float>("map_tile_size"));
  sf::Vector2f box_half_size(box_size.x / 2, box_size.y / 2);
  box_position.x = (location.x - box_half_size.x);
  box_position.y = (location.y - box_half_size.y);
  box_shape.setPosition(box_position);
  box_shape.setSize(box_size);
  box_shape.setOutlineColor(fgColor);
  box_shape.setOutlineThickness(Settings.get<float>("tile_highlight_border_width"));
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
  sf::Color tile_color{ sf::Color::White };

  // Wall colors.
  sf::Color wall_n_color_w{ sf::Color::White };
  sf::Color wall_n_color{ sf::Color::White };
  sf::Color wall_n_color_e{ sf::Color::White };

  sf::Color wall_e_color_n{ sf::Color::White };
  sf::Color wall_e_color{ sf::Color::White };
  sf::Color wall_e_color_s{ sf::Color::White };

  sf::Color wall_s_color_w{ sf::Color::White };
  sf::Color wall_s_color{ sf::Color::White };
  sf::Color wall_s_color_e{ sf::Color::White };

  sf::Color wall_w_color_n{ sf::Color::White };
  sf::Color wall_w_color{ sf::Color::White };
  sf::Color wall_w_color_s{ sf::Color::White };

  // Full tile size.
  float ts(Settings.get<float>("map_tile_size"));

  // Half of the tile size.
  float half_ts(Settings.get<float>("map_tile_size") * 0.5f);

  // Wall size (configurable).
  float ws(Settings.get<float>("map_tile_size") * 0.4f);

  // Adjacent tiles
  MapTile const& adjacent_tile_n = get_adjacent_tile(Direction::North);
  MapTile const& adjacent_tile_e = get_adjacent_tile(Direction::East);
  MapTile const& adjacent_tile_s = get_adjacent_tile(Direction::South);
  MapTile const& adjacent_tile_w = get_adjacent_tile(Direction::West);

  // Tile vertices.
  sf::Vector2f location(m_coords.x * ts,
                        m_coords.y * ts);
  sf::Vector2f vTileN(location.x, location.y - half_ts);
  sf::Vector2f vTileNE(location.x + half_ts, location.y - half_ts);
  sf::Vector2f vTileE(location.x + half_ts, location.y);
  sf::Vector2f vTileSE(location.x + half_ts, location.y + half_ts);
  sf::Vector2f vTileS(location.x, location.y + half_ts);
  sf::Vector2f vTileSW(location.x - half_ts, location.y + half_ts);
  sf::Vector2f vTileW(location.x - half_ts, location.y);
  sf::Vector2f vTileNW(location.x - half_ts, location.y - half_ts);

  sf::Vector2u tile_coords = this->get_tile_sheet_coords(0);

  if (use_lighting)
  {
    tile_color = get_light_level();

    wall_n_color = get_wall_light_level(Direction::North);
    wall_n_color_w = average(wall_n_color, adjacent_tile_w.get_wall_light_level(Direction::North));
    wall_n_color_e = average(wall_n_color, adjacent_tile_e.get_wall_light_level(Direction::North));

    wall_e_color = get_wall_light_level(Direction::East);
    wall_e_color_n = average(wall_e_color, adjacent_tile_n.get_wall_light_level(Direction::East));
    wall_e_color_s = average(wall_e_color, adjacent_tile_s.get_wall_light_level(Direction::East));


    wall_s_color = get_wall_light_level(Direction::South);
    wall_s_color_w = average(wall_s_color, adjacent_tile_w.get_wall_light_level(Direction::South));
    wall_s_color_e = average(wall_s_color, adjacent_tile_e.get_wall_light_level(Direction::South));

    wall_w_color = get_wall_light_level(Direction::West);
    wall_w_color_n = average(wall_w_color, adjacent_tile_n.get_wall_light_level(Direction::West));
    wall_w_color_s = average(wall_w_color, adjacent_tile_s.get_wall_light_level(Direction::West));
  }

  // NORTH WALL
  if (n_is_empty && player_sees_n_wall)
  {
    sf::Vector2f vS(vTileN.x, vTileN.y + ws);
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

    TileSheet::add_gradient_quad(vertices, tile_coords, wall_n_color_w, vTileNW, wall_n_color, vTileN, wall_n_color, vS, wall_n_color_w, vSW);
    TileSheet::add_gradient_quad(vertices, tile_coords, wall_n_color, vTileN, wall_n_color_e, vTileNE, wall_n_color_e, vSE, wall_n_color, vS);
  }

  // EAST WALL
  if (e_is_empty && player_sees_e_wall)
  {
    sf::Vector2f vW(vTileE.x - ws, vTileE.y);
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

    TileSheet::add_gradient_quad(vertices, tile_coords, wall_e_color_n, vNW, wall_e_color_n, vTileNE, wall_e_color, vTileE, wall_e_color, vW);
    TileSheet::add_gradient_quad(vertices, tile_coords, wall_e_color, vW, wall_e_color, vTileE, wall_e_color_s, vTileSE, wall_e_color_s, vSW);
  }

  // SOUTH WALL
  if (s_is_empty && player_sees_s_wall)
  {
    sf::Vector2f vN(vTileS.x, vTileS.y - ws);
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

    TileSheet::add_gradient_quad(vertices, tile_coords, wall_s_color_w, vNW, wall_s_color, vN, wall_s_color, vTileS, wall_s_color_w, vTileSW);
    TileSheet::add_gradient_quad(vertices, tile_coords, wall_s_color, vN, wall_s_color_e, vNE, wall_s_color_e, vTileSE, wall_s_color, vTileS);
  }

  // WEST WALL
  if (w_is_empty && player_sees_w_wall)
  {
    sf::Vector2f vE(vTileW.x + ws, vTileW.y);
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

    TileSheet::add_gradient_quad(vertices, tile_coords, wall_w_color_n, vTileNW, wall_w_color_n, vNE, wall_w_color, vE, wall_w_color, vTileW);
    TileSheet::add_gradient_quad(vertices, tile_coords, wall_w_color, vTileW, wall_w_color, vE, wall_w_color_s, vSE, wall_w_color_s, vTileSW);
  }
}

sf::Vector2f MapTile::get_pixel_coords(int x, int y)
{
  return sf::Vector2f(static_cast<float>(x) * Settings.get<float>("map_tile_size"), static_cast<float>(y) * Settings.get<float>("map_tile_size"));
}

sf::Vector2f MapTile::get_pixel_coords(sf::Vector2i tile)
{
  return get_pixel_coords(tile.x, tile.y);
}

// === PROTECTED METHODS ======================================================

MapTile::MapTile(sf::Vector2i coords, Metadata& metadata, MapId map_id)
  : 
  m_map_id{ map_id },
  m_coords{ coords },  
  m_pMetadata{ &metadata },
  m_ambient_light_color{ sf::Color(192, 192, 192, 255) }
{
  if (!initialized)
  {
    // TODO: any static class initialization that must be performed
    initialized = true;
  }

  // Floor is created out here, or else the pImpl would need the
  // "this" pointer passed in.
  /// @todo The type of this floor should eventually be specified as
  ///       part of the constructor.
  m_floor = TM.create_floor(this);
}

MapTile const& MapTile::get_adjacent_tile(Direction direction) const
{
  sf::Vector2i coords = get_coords();
  Map const& map = MF.get(get_map_id());
  MapTile const& tile = *this;

  switch (direction)
  {
  case Direction::North:
    return map.get_tile(coords.x, coords.y - 1);

  case Direction::South:
    return map.get_tile(coords.x, coords.y + 1);

  case Direction::West:
    return map.get_tile(coords.x - 1, coords.y);

  case Direction::East:
    return map.get_tile(coords.x + 1, coords.y);

  case Direction::Northwest:
    return map.get_tile(coords.x - 1, coords.y - 1);

  case Direction::Northeast:
    return map.get_tile(coords.x + 1, coords.y - 1);

  case Direction::Southwest:
    return map.get_tile(coords.x - 1, coords.y + 1);

  case Direction::Southeast:
    return map.get_tile(coords.x + 1, coords.y + 1);

  default:
    return tile;
  }
}