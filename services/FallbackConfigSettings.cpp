#include "stdafx.h"

#include "services/FallbackConfigSettings.h"
#include "Types/Color.h"

FallbackConfigSettings::FallbackConfigSettings()
{
  /// @todo These settings should be read from an XML file.
  set("debug_show_thing_ids", true);
  set("window_border_color", Color(96, 48, 32));
  set("window_border_color", Color(96, 48, 32));
  set("window_focused_border_color", Color(192, 96, 64));
  set("window_bg_color", Color(48, 24, 16));
  set("window_focused_bg_color", Color(48, 24, 16));
  set("font_name_default", "Dustismo_Roman");
  set("font_name_bold", "Dustismo_Roman_Bold");
  set("font_name_mono", "DejaVuSansMono");
  set("font_name_unicode", "FreeMono");
  set("text_dim_color", Color(192, 192, 128));
  set("text_color", Color(224, 224, 160));
  set("text_warning_color", Color(255, 255, 0));
  set("text_danger_color", Color(255, 0, 0));
  set("text_highlight_color", Color(255, 255, 255));
  set("text_default_size", 16);
  set("text_mono_default_size", 12);
  set("cursor_border_color", Color(255, 255, 240));
  set("cursor_bg_color", Color(255, 255, 240, 32));
  set("window_border_width", 2);
  set("window_handle_size", 3);
  set("window_text_offset_x", 3);
  set("window_text_offset_y", 3);
  set("tile_highlight_border_width", 2);
  set("inventory_area_width", 300);
  set("messagelog_area_height", 200);
  set("status_area_height", 90);
  set("map_tile_size", 32);
  set("tilesheet_texture_size", 1024);

  set("player_name", "Clongus Burpo");
}

FallbackConfigSettings::~FallbackConfigSettings()
{
  //dtor
}