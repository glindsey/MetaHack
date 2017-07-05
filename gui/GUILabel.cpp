#include "stdafx.h"

#include "GUILabel.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"
#include "types/Color.h"

namespace metagui
{
  Label::Label(Desktop& desktop, std::string name, IntVec2 location) :
    GUIObject(desktop, name, {}, location, { 0, 0 })
  {}

  Label::~Label()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void Label::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = S<IConfigSettings>();
    float line_spacing_y = the_default_font.getLineSpacing(config.get("text-default-size"));

    // Text offsets relative to the background rectangle.
    RealVec2 offset{ 3.0f, 3.0f };

    /// @todo Flesh this out; right now it is EXTREMELY rudimentary.
    ///       All it does is render a straight text string; no word wrap,
    ///       token replacement, etc. is performed. It needs to do the
    ///       following:
    ///         - Handle word wrapping correctly
    ///         - Handle drop-in tokens for changing text color/style
    ///         - Handle proper text alignment (left/centered/right)
    std::string str = getText();
    if (!str.empty())
    {
      sf::Text text{ str, the_default_font, static_cast<unsigned int>(config.get("text-default-size")) };
      auto text_color = config.get("text-color").get<Color>();
      text.setColor(text_color);
      text.setPosition(offset);

      // Resize vertically if "resize_to_fit" is true.
      // Must do prior to drawing, as setSize() deletes and recreates the
      // background texture when called.
      /// @todo Flesh this out as well.
      if (getFlag("resize_to_fit") == true)
      {
        auto current_x_size = getSize().y;
        auto new_y_size = text.getGlobalBounds().height;
        setSize({ current_x_size, static_cast<unsigned int>(new_y_size) });
      }

      // Draw to the background texture.
      texture.draw(text);
    }
  }
}; // end namespace metagui