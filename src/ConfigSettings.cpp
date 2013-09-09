#include "ConfigSettings.h"

std::unique_ptr<ConfigSettings> ConfigSettings::instance_;

ConfigSettings::ConfigSettings()
{
  window_border_color         = sf::Color(255, 255, 240, 255);
  window_focused_border_color = sf::Color(255, 255,  64, 255);
  window_bg_color             = sf::Color( 48,  24,   8, 192);
  font_name_default           = "Dustismo_Roman";
  font_name_bold              = "Dustismo_Roman_Bold";
  font_name_mono              = "DejaVuSansMono";
  text_color                  = sf::Color(255, 255, 240, 255);
  text_warning_color          = sf::Color(255, 255,   0, 255);
  text_danger_color           = sf::Color(255,   0,   0, 255);
  text_highlight_color        = sf::Color(255, 255,  64, 255);
  text_default_size           = 16;
  text_mono_default_size      = 12;
  cursor_border_color         = sf::Color(255, 255, 240, 255);
  cursor_bg_color             = sf::Color(255, 255, 240,  32);
  window_border_width         = 2;
  tile_highlight_border_width = 2;
  inventory_area_width        = 220;
  status_area_height          = 80;
  map_tile_size               = 32;
}

ConfigSettings::~ConfigSettings()
{
  //dtor
}

ConfigSettings& ConfigSettings::instance()
{
  if (instance_ == nullptr)
  {
    instance_.reset(new ConfigSettings());
  }

  return *(instance_.get());
}
