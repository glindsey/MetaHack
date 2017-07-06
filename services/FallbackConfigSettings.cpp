#include "stdafx.h"

#include "services/FallbackConfigSettings.h"
#include "types/Color.h"
#include "types/Vec2.h"

FallbackConfigSettings::FallbackConfigSettings()
{
  /// @todo These settings should be read from an XML file.
  set("debug-show-thing-ids", true);
  set("window-border-color", Color(96, 48, 32));
  set("window-focused-border-color", Color(192, 96, 64));
  set("window-bg-color", Color(48, 24, 16));
  set("window-focused-bg-color", Color(48, 24, 16));
  set("font-name-default", "Dustismo_Roman");
  set("font-name-bold", "Dustismo_Roman_Bold");
  set("font-name-mono", "DejaVuSansMono");
  set("font-name-unicode", "FreeMono");
  set("text-dim-color", Color(192, 192, 128));
  set("text-color", Color(224, 224, 160));
  set("text-warning-color", Color(255, 255, 0));
  set("text-danger-color", Color(255, 0, 0));
  set("text-highlight-color", Color(255, 255, 255));
  set("text-default-size", 16);
  set("text-mono-default-size", 12);
  set("cursor-border-color", Color(255, 255, 240));
  set("cursor-bg-color", Color(255, 255, 240, 32));
  set("window-border-width", 2);
  set("window-handle-size", 3);
  set("window-text-offset", RealVec2(3, 3));
  set("tile-highlight-border-width", 2);
  set("inventory-area-width", 300);
  set("messagelog-area-height", 200);
  set("status-area-height", 90);
  set("map-ascii-tile-size", UintVec2(12, 12));
  set("map-graphics-tile-size", UintVec2(32, 32));
  set("map-tile-size", UintVec2(24, 24));
  set("tilesheet-texture-size", UintVec2(1024, 1024));
  set("ascii-tiles-filename", "Unknown_curses_12x12.png");

  set("player-name", "Clongus Burpo");
}

FallbackConfigSettings::~FallbackConfigSettings()
{
  //dtor
}