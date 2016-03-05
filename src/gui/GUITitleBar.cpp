#include "stdafx.h"

#include "gui/GUITitleBar.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  TitleBar::TitleBar(std::string name)
    :
    Object(name, sf::Vector2i(0, 0))
  {}

  TitleBar::~TitleBar()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void TitleBar::render_self_before_children_(sf::RenderTexture& texture, int frame)
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
  }
}; // end namespace metagui