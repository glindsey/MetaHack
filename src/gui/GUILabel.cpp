#include "stdafx.h"

#include "gui/GUILabel.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

GUILabel::GUILabel(std::string name, sf::Vector2i location)
  :
  GUIObject(name, location)
{}

GUILabel::~GUILabel()
{
  //dtor
}

// === PROTECTED METHODS ======================================================
bool GUILabel::_render_self(sf::RenderTexture& texture, int frame)
{
  float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

  // Text offsets relative to the background rectangle.
  sf::Vector2f offset{ 3.0f, 3.0f };

  /// @todo Flesh this out; right now it is EXTREMELY rudimentary
  std::string str = get_text();
  if (!str.empty())
  {
    sf::Text text{ str, the_default_font, Settings.get<unsigned int>("text_default_size") };
    text.setColor(Settings.get<sf::Color>("text_color"));
    text.setPosition(offset);
    texture.draw(text);
  }

  return true;
}