#include "StatusArea.h"

#include <boost/lexical_cast.hpp>
#include <locale>

#include "gui/GUILabel.h"
#include "App.h"
#include "ConfigSettings.h"
#include "GameState.h"
#include "Thing.h"
#include "ThingManager.h"

struct StatusArea::Impl
{
  bool dummy;
};

StatusArea::StatusArea(sf::IntRect dimensions)
  : GUIPane(dimensions),
  pImpl(NEW Impl())
{
  //ctor
  //std::function<std::string()> label_function = std::bind(&StatusArea::get_test_label, this);
  //GUILabel* label = NEW GUILabel( {0, 0, dimensions.width / 2, dimensions.height / 2}, label_function );
  //add_child(label);
}

StatusArea::~StatusArea()
{
  //dtor
}

EventResult StatusArea::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

std::string StatusArea::_render_contents(sf::RenderTarget& target, int frame)
{
  sf::IntRect pane_dims = get_dimensions();
  ThingRef player = GAME.get_player();
  sf::Vector2f origin(3, 3);
  sf::Color text_color = Settings.get<sf::Color>("text_color");
  sf::Color text_dim_color = Settings.get<sf::Color>("text_dim_color");
  sf::Color text_warning_color = Settings.get<sf::Color>("text_warning_color");
  sf::Color text_danger_color = Settings.get<sf::Color>("text_danger_color");
  float line_spacing = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size")) + 3.0f;
  float attrib_spacing = 75.0f;

  // Text offsets relative to the background rectangle.
  sf::Text render_text;
  render_text.setFont(the_default_font);
  render_text.setColor(text_color);
  render_text.setCharacterSize(Settings.get<unsigned int>("text_default_size"));
  render_text.setPosition(origin.x, origin.y);

  if (player != MU)
  {
    // Render player name
    std::string name = player->get_proper_name();
    name[0] = std::toupper(name[0], std::locale());

    std::string type = player->get_display_name();
    type[0] = std::toupper(type[0], std::locale());

    render_text.setString(name + " the " + type);
    target.draw(render_text);

    // Render HP
    render_text.setFont(the_default_mono_font);
    render_text.setColor(text_dim_color);
    render_text.setPosition(origin.x, origin.y + line_spacing);
    render_text.setString("HP");
    target.draw(render_text);

    int hp = player->get_property<int>("hp");
    int max_hp = player->get_property<int>("maxhp");

    float hp_percentage = static_cast<float>(hp) / static_cast<float>(max_hp);

    if (hp_percentage > 0.6)
    {
      render_text.setColor(text_color);
    }
    else if (hp_percentage > 0.3)
    {
      render_text.setColor(text_warning_color);
    }
    else
    {
      render_text.setColor(text_danger_color);
    }

    std::string hp_string = boost::lexical_cast<std::string>(hp) + "/" + boost::lexical_cast<std::string>(max_hp);

    render_text.setPosition(origin.x + 30, origin.y + line_spacing);
    render_text.setString(hp_string);
    target.draw(render_text);

    // Render attributes
    render_attribute(target, "XP", "xp", { origin.x, origin.y + (2 * line_spacing) });
    render_attribute(target, "STR", "attribute_strength", { origin.x + (0 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "VIG", "attribute_vigilance", { origin.x + (1 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "END", "attribute_endurance", { origin.x + (2 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "CHA", "attribute_charisma", { origin.x + (3 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "INT", "attribute_intelligence", { origin.x + (4 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "AGI", "attribute_agility", { origin.x + (5 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "LUC", "attribute_luck", { origin.x + (6 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "VIT", "attribute_vitality", { origin.x + (7 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(target, "AUR", "attribute_aura", { origin.x + (8 * attrib_spacing), origin.y + (3 * line_spacing) });
  }

  return "";
}

void StatusArea::render_attribute(sf::RenderTarget& target, std::string abbrev, std::string name, sf::Vector2f location)
{
  sf::Text render_text;
  sf::Color text_color = Settings.get<sf::Color>("text_color");
  sf::Color text_dim_color = Settings.get<sf::Color>("text_dim_color");
  ThingRef player = GAME.get_player();

  // Render STR
  render_text.setFont(the_default_mono_font);
  render_text.setColor(text_dim_color);
  render_text.setCharacterSize(Settings.get<unsigned int>("text_default_size"));
  render_text.setPosition(location.x, location.y);
  render_text.setString(abbrev + ":");
  target.draw(render_text);

  std::string attr_string = boost::lexical_cast<std::string>(player->get_property<int>(name));

  render_text.setColor(text_color);
  render_text.setPosition(location.x + 40, location.y);
  render_text.setString(attr_string);
  target.draw(render_text);
}

std::string StatusArea::get_test_label()
{
  return "Testing GUILabel";
}