#include "stdafx.h"

#include "IConfigSettings.h"
#include "MapTileStandard2DView.h"
#include "RNGUtils.h"
#include "Service.h"

#include "ShaderEffect.h"

MapTileStandard2DView::MapTileStandard2DView(MapTile& map_tile)
  :
  MapTileView(map_tile),
  m_tile_offset{ pick_uniform(0, 4) }
{
}

Vec2u MapTileStandard2DView::get_tile_sheet_coords() const
{
  /// @todo Deal with selecting one of the other tiles.
  Vec2u start_coords = get_map_tile().get_metadata().get_tile_coords();
  Vec2u tile_coords(start_coords.x + m_tile_offset, start_coords.y);
  return tile_coords;
}

void MapTileStandard2DView::add_tile_vertices(ThingId viewer,
                                              sf::VertexArray& seen_vertices,
                                              sf::VertexArray& memory_vertices)
{
  auto& tile = get_map_tile();
  auto coords = tile.get_coords();
  auto& x = coords.x;
  auto& y = coords.y;

  bool this_is_empty = tile.is_empty_space();
  bool nw_is_empty = (viewer->can_see({ x - 1, y - 1 }) && tile.get_adjacent_tile(Direction::Northwest).is_empty_space());
  bool n_is_empty = (viewer->can_see({ x, y - 1 }) && tile.get_adjacent_tile(Direction::North).is_empty_space());
  bool ne_is_empty = (viewer->can_see({ x + 1, y - 1 }) && tile.get_adjacent_tile(Direction::Northeast).is_empty_space());
  bool e_is_empty = (viewer->can_see({ x + 1, y }) && tile.get_adjacent_tile(Direction::East).is_empty_space());
  bool se_is_empty = (viewer->can_see({ x + 1, y + 1 }) && tile.get_adjacent_tile(Direction::Southeast).is_empty_space());
  bool s_is_empty = (viewer->can_see({ x, y + 1 }) && tile.get_adjacent_tile(Direction::South).is_empty_space());
  bool sw_is_empty = (viewer->can_see({ x - 1, y + 1 }) && tile.get_adjacent_tile(Direction::Southwest).is_empty_space());
  bool w_is_empty = (viewer->can_see({ x - 1, y }) && tile.get_adjacent_tile(Direction::West).is_empty_space());

  if (viewer->can_see(coords))
  {
    if (this_is_empty)
    {
      add_tile_floor_vertices(seen_vertices);
    }
    else
    {
      add_wall_vertices_to(seen_vertices, true,
                           nw_is_empty, n_is_empty,
                           ne_is_empty, e_is_empty,
                           se_is_empty, s_is_empty,
                           sw_is_empty, w_is_empty);
    }
  }
  else
  {
    viewer->add_memory_vertices_to(memory_vertices, { x, y });
  }
}

void MapTileStandard2DView::add_tile_floor_vertices(sf::VertexArray& vertices)
{
  auto& config = Service<IConfigSettings>::get();

  auto& tile = get_map_tile();
  auto& coords = tile.get_coords();
  auto& tileN = tile.get_adjacent_tile(Direction::North);
  auto& tileNE = tile.get_adjacent_tile(Direction::Northeast);
  auto& tileE = tile.get_adjacent_tile(Direction::East);
  auto& tileSE = tile.get_adjacent_tile(Direction::Southeast);
  auto& tileS = tile.get_adjacent_tile(Direction::South);
  auto& tileSW = tile.get_adjacent_tile(Direction::Southwest);
  auto& tileW = tile.get_adjacent_tile(Direction::West);
  auto& tileNW = tile.get_adjacent_tile(Direction::Northwest);

  sf::Vertex new_vertex;
  float ts = config.get<float>("map_tile_size");
  float half_ts = ts * 0.5f;

  sf::Color colorN{ tileN.get_light_level() };
  sf::Color colorNE{ tileNE.get_light_level() };
  sf::Color colorE{ tileE.get_light_level() };
  sf::Color colorSE{ tileSE.get_light_level() };
  sf::Color colorS{ tileS.get_light_level() };
  sf::Color colorSW{ tileSW.get_light_level() };
  sf::Color colorW{ tileW.get_light_level() };
  sf::Color colorNW{ tileNW.get_light_level() };

  sf::Color light = tile.get_light_level();
  sf::Color lightN = average(light, colorN);
  sf::Color lightNE = average(light, colorN, colorNE, colorE);
  sf::Color lightE = average(light, colorE);
  sf::Color lightSE = average(light, colorE, colorSE, colorS);
  sf::Color lightS = average(light, colorS);
  sf::Color lightSW = average(light, colorS, colorSW, colorW);
  sf::Color lightW = average(light, colorW);
  sf::Color lightNW = average(light, colorW, colorNW, colorN);

  Vec2f location{ coords.x * ts, coords.y * ts };
  Vec2f vNE{ location.x + half_ts, location.y - half_ts };
  Vec2f vSE{ location.x + half_ts, location.y + half_ts };
  Vec2f vSW{ location.x - half_ts, location.y + half_ts };
  Vec2f vNW{ location.x - half_ts, location.y - half_ts };

  Vec2u tile_coords = get_tile_sheet_coords();

  the_tilesheet.add_gradient_quad(vertices, tile_coords,
                                  vNW, vNE,
                                  vSW, vSE,
                                  lightNW, lightN, lightNE,
                                  lightW, light, lightE,
                                  lightSW, lightS, lightSE);
}

void MapTileStandard2DView::add_wall_vertices_to(sf::VertexArray& vertices,
                                                 bool use_lighting,
                                                 bool nw_is_empty, bool n_is_empty,
                                                 bool ne_is_empty, bool e_is_empty,
                                                 bool se_is_empty, bool s_is_empty,
                                                 bool sw_is_empty, bool w_is_empty)
{
  auto& config = Service<IConfigSettings>::get();
  auto map_tile_size = config.get<float>("map_tile_size");

  // This tile.
  MapTile& tile = get_map_tile();

  // Checks to see N/S/E/W walls.
  bool player_sees_n_wall{ false };
  bool player_sees_s_wall{ false };
  bool player_sees_e_wall{ false };
  bool player_sees_w_wall{ false };

  // Player.
  ThingId player = GAME.get_player();

  if (player != ThingId::Mu())
  {
    auto player_tile = player->get_maptile();
    auto player_coords = player_tile->get_coords();
    auto tile_coords = tile.get_coords();
    if (player_tile != nullptr)
    {
      player_sees_n_wall = (player_coords.y <= tile_coords.y);
      player_sees_s_wall = (player_coords.y >= tile_coords.y);
      player_sees_e_wall = (player_coords.x >= tile_coords.x);
      player_sees_w_wall = (player_coords.x <= tile_coords.x);
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
  float ts(map_tile_size);

  // Half of the tile size.
  float half_ts(map_tile_size * 0.5f);

  // Wall size (configurable).
  float ws(map_tile_size * 0.4f);

  // Adjacent tiles
  MapTile const& adjacent_tile_n = tile.get_adjacent_tile(Direction::North);
  MapTile const& adjacent_tile_e = tile.get_adjacent_tile(Direction::East);
  MapTile const& adjacent_tile_s = tile.get_adjacent_tile(Direction::South);
  MapTile const& adjacent_tile_w = tile.get_adjacent_tile(Direction::West);

  // Tile vertices.
  auto tile_coords = tile.get_coords();
  Vec2f location(tile_coords.x * ts,
                 tile_coords.y * ts);
  Vec2f vTileN(location.x, location.y - half_ts);
  Vec2f vTileNE(location.x + half_ts, location.y - half_ts);
  Vec2f vTileE(location.x + half_ts, location.y);
  Vec2f vTileSE(location.x + half_ts, location.y + half_ts);
  Vec2f vTileS(location.x, location.y + half_ts);
  Vec2f vTileSW(location.x - half_ts, location.y + half_ts);
  Vec2f vTileW(location.x - half_ts, location.y);
  Vec2f vTileNW(location.x - half_ts, location.y - half_ts);

  Vec2u tile_sheet_coords = this->get_tile_sheet_coords();

  if (use_lighting)
  {
    tile_color = tile.get_light_level();

    if (player_sees_n_wall)
    {
      wall_n_color = tile.get_wall_light_level(Direction::North);
      wall_n_color_w = average(wall_n_color, adjacent_tile_w.get_wall_light_level(Direction::North));
      wall_n_color_e = average(wall_n_color, adjacent_tile_e.get_wall_light_level(Direction::North));
    }
    else
    {
      wall_n_color = sf::Color::Black;
      wall_n_color_w = sf::Color::Black;
      wall_n_color_e = sf::Color::Black;
    }

    if (player_sees_e_wall)
    {
      wall_e_color = tile.get_wall_light_level(Direction::East);
      wall_e_color_n = average(wall_e_color, adjacent_tile_n.get_wall_light_level(Direction::East));
      wall_e_color_s = average(wall_e_color, adjacent_tile_s.get_wall_light_level(Direction::East));
    }
    else
    {
      wall_e_color = sf::Color::Black;
      wall_e_color_n = sf::Color::Black;
      wall_e_color_s = sf::Color::Black;
    }

    if (player_sees_s_wall)
    {
      wall_s_color = tile.get_wall_light_level(Direction::South);
      wall_s_color_w = average(wall_s_color, adjacent_tile_w.get_wall_light_level(Direction::South));
      wall_s_color_e = average(wall_s_color, adjacent_tile_e.get_wall_light_level(Direction::South));
    }
    else
    {
      wall_s_color = sf::Color::Black;
      wall_s_color_w = sf::Color::Black;
      wall_s_color_e = sf::Color::Black;
    }

    if (player_sees_w_wall)
    {
      wall_w_color = tile.get_wall_light_level(Direction::West);
      wall_w_color_n = average(wall_w_color, adjacent_tile_n.get_wall_light_level(Direction::West));
      wall_w_color_s = average(wall_w_color, adjacent_tile_s.get_wall_light_level(Direction::West));
    }
    else
    {
      wall_w_color = sf::Color::Black;
      wall_w_color_n = sf::Color::Black;
      wall_w_color_s = sf::Color::Black;
    }
  }

  // NORTH WALL
  if (n_is_empty)
  {
    Vec2f vS(vTileN.x, vTileN.y + ws);
    Vec2f vSW(vTileNW.x, vTileNW.y + ws);
    if (w_is_empty)
    {
      vSW.x += ws;
    }
    else if (!nw_is_empty)
    {
      vSW.x -= ws;
    }
    Vec2f vSE(vTileNE.x, vTileNE.y + ws);
    if (e_is_empty)
    {
      vSE.x -= ws;
    }
    else if (!ne_is_empty)
    {
      vSE.x += ws;
    }

    the_tilesheet.add_gradient_quad(vertices, tile_sheet_coords,
                                    vTileNW, vTileNE,
                                    vSW, vSE,
                                    wall_n_color_w, wall_n_color, wall_n_color_e,
                                    wall_n_color_w, wall_n_color, wall_n_color_e,
                                    wall_n_color_w, wall_n_color, wall_n_color_e);
  }

  // EAST WALL
  if (e_is_empty)
  {
    Vec2f vW(vTileE.x - ws, vTileE.y);
    Vec2f vNW(vTileNE.x - ws, vTileNE.y);
    if (n_is_empty)
    {
      vNW.y += ws;
    }
    else if (!ne_is_empty)
    {
      vNW.y -= ws;
    }
    Vec2f vSW(vTileSE.x - ws, vTileSE.y);
    if (s_is_empty)
    {
      vSW.y -= ws;
    }
    else if (!se_is_empty)
    {
      vSW.y += ws;
    }

    the_tilesheet.add_gradient_quad(vertices, tile_sheet_coords,
                                    vNW, vTileNE,
                                    vSW, vTileSE,
                                    wall_e_color_n, wall_e_color_n, wall_e_color_n,
                                    wall_e_color, wall_e_color, wall_e_color,
                                    wall_e_color_s, wall_e_color_s, wall_e_color_s);
  }

  // SOUTH WALL
  if (s_is_empty)
  {
    Vec2f vN(vTileS.x, vTileS.y - ws);
    Vec2f vNW(vTileSW.x, vTileSW.y - ws);
    if (w_is_empty)
    {
      vNW.x += ws;
    }
    else if (!sw_is_empty)
    {
      vNW.x -= ws;
    }
    Vec2f vNE(vTileSE.x, vTileSE.y - ws);
    if (e_is_empty)
    {
      vNE.x -= ws;
    }
    else if (!se_is_empty)
    {
      vNE.x += ws;
    }

    the_tilesheet.add_gradient_quad(vertices, tile_sheet_coords,
                                    vNW, vNE,
                                    vTileSW, vTileSE,
                                    wall_s_color_w, wall_s_color, wall_s_color_e,
                                    wall_s_color_w, wall_s_color, wall_s_color_e,
                                    wall_s_color_w, wall_s_color, wall_s_color_e);
  }

  // WEST WALL
  if (w_is_empty)
  {
    Vec2f vE(vTileW.x + ws, vTileW.y);
    Vec2f vNE(vTileNW.x + ws, vTileNW.y);
    if (n_is_empty)
    {
      vNE.y += ws;
    }
    else if (!nw_is_empty)
    {
      vNE.y -= ws;
    }
    Vec2f vSE(vTileSW.x + ws, vTileSW.y);
    if (s_is_empty)
    {
      vSE.y -= ws;
    }
    else if (!sw_is_empty)
    {
      vSE.y += ws;
    }

    the_tilesheet.add_gradient_quad(vertices, tile_sheet_coords,
                                    vTileNW, vNE,
                                    vTileSW, vSE,
                                    wall_w_color_n, wall_w_color_n, wall_w_color_n,
                                    wall_w_color, wall_w_color, wall_w_color,
                                    wall_w_color_s, wall_w_color_s, wall_w_color_s);
  }
}

void MapTileStandard2DView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
