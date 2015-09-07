#include "StatusArea.h"

#include <boost/lexical_cast.hpp>
#include <locale>

#include "App.h"
#include "ConfigSettings.h"
#include "Thing.h"
#include "ThingManager.h"

struct StatusArea::Impl
{
  bool dummy;
};

StatusArea::StatusArea(sf::IntRect dimensions)
  : GUIPane(dimensions),
    pImpl(new Impl())
{
  //ctor
}

StatusArea::~StatusArea()
{
  //dtor
}

EventResult StatusArea::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

std::string StatusArea::render_contents(int frame)
{
  // Dimensions of the pane.
  sf::IntRect pane_dims = get_dimensions();

  // Our render texture.
  sf::RenderTexture& bg_texture = get_bg_texture();

  // The player.
  ThingRef player = TM.get_player();

  int line_spacing = the_default_font.getLineSpacing(Settings.text_default_size);

  // Text offsets relative to the background rectangle.
  sf::Text render_text;
  render_text.setFont(the_default_font);
  render_text.setCharacterSize(Settings.text_default_size);
  render_text.setColor(Settings.text_color);
  render_text.setPosition(3, 3);

  if (player != TM.get_mu())
  {
    std::string name = player->get_proper_name();
    name[0] = std::toupper(name[0], std::locale());

    std::string type = player->get_pretty_name();
    type[0] = std::toupper(type[0], std::locale());

    render_text.setString(name + " the " + type);
    bg_texture.draw(render_text);

    render_text.setFont(the_default_mono_font);
    render_text.setPosition(3, 23);
    render_text.setString("HP");
    bg_texture.draw(render_text);

    int hp = player->get_attributes().get(Attribute::HP);
    int max_hp = player->get_attributes().get(Attribute::MaxHP);

    float hp_percentage = static_cast<float>(hp) / static_cast<float>(max_hp);

    if (hp_percentage > 0.6)
    {
      render_text.setColor(Settings.text_color);
    }
    else if (hp_percentage > 0.3)
    {
      render_text.setColor(Settings.text_warning_color);
    }
    else
    {
      render_text.setColor(Settings.text_danger_color);
    }

    std::string hp_string = boost::lexical_cast<std::string>(hp)+
      "/" + boost::lexical_cast<std::string>(max_hp);

    render_text.setPosition(33, 23);
    render_text.setString(hp_string);
    bg_texture.draw(render_text);
  }

  return "";
}
