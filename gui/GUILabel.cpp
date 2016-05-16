#include "stdafx.h"

#include "GUILabel.h"

#include "App.h"
#include "ConfigSettings.h"

namespace metagui
{
  Label::Label(StringKey name, sf::Vector2i location)
    :
    Object(name, location)
  {}

  Label::~Label()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void Label::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
    float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

    // Text offsets relative to the background rectangle.
    sf::Vector2f offset{ 3.0f, 3.0f };

    /// @todo Flesh this out; right now it is EXTREMELY rudimentary
    StringDisplay str = get_text();
    if (!str.isEmpty())
    {
      sf::Text text{ str, the_default_font, Settings.get<unsigned int>("text_default_size") };
      text.setColor(Settings.get<sf::Color>("text_color"));
      text.setPosition(offset);
      texture.draw(text);
    }
  }
}; // end namespace metagui