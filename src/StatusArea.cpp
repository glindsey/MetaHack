#include "stdafx.h"

#include "StatusArea.h"

#include "GUILabel.h"
#include "App.h"
#include "GameState.h"
#include "IConfigSettings.h"
#include "Service.h"
#include "Entity.h"
#include "EntityPool.h"

struct StatusArea::Impl
{
  bool dummy;
};

StatusArea::StatusArea(std::string name, sf::IntRect dimensions)
  : metagui::Window(name, dimensions),
  pImpl(NEW Impl())
{
  //ctor
  //std::function<std::string()> label_function = std::bind(&StatusArea::get_test_label, this);
  //GUILabel* label = NEW GUILabel( {0, 0, dimensions.width / 2, dimensions.height / 2}, label_function );
  //addChild(label);
}

StatusArea::~StatusArea()
{
  //dtor
}

void StatusArea::drawContents_(sf::RenderTexture& texture, int frame)
{
  auto& config = Service<IConfigSettings>::get();

  sf::IntRect pane_dims = getRelativeDimensions();
  EntityId player = GAME.get_player();
  RealVec2 origin(config.get("window_text_offset_x").as<float>(),
               config.get("window_text_offset_y").as<float>());
  sf::Color text_color = config.get("text_color").as<Color>();
  sf::Color text_dim_color = config.get("text_dim_color").as<Color>();
  sf::Color text_warning_color = config.get("text_warning_color").as<Color>();
  sf::Color text_danger_color = config.get("text_danger_color").as<Color>();
  auto text_default_size = config.get("text_default_size").as<int32_t>();
  float line_spacing = the_default_font.getLineSpacing(text_default_size) + 3.0f;
  float attrib_spacing = 75.0f;

  // Text offsets relative to the background rectangle.
  sf::Text render_text;
  render_text.setFont(the_default_font);
  render_text.setColor(text_color);
  render_text.setCharacterSize(text_default_size);
  render_text.setPosition({ origin.x, origin.y });

  if (player != EntityId::Mu())
  {
    // Render player name
    std::string name = player->get_proper_name();
    name[0] = std::toupper(name[0], std::locale());

    std::string type = player->get_display_name();
    type[0] = std::toupper(type[0], std::locale());

    render_text.setString(name + " the " + type);
    texture.draw(render_text);

    // Render HP
    render_text.setFont(the_default_mono_font);
    render_text.setColor(text_dim_color);
    render_text.setPosition({ origin.x, origin.y + line_spacing });
    render_text.setString("HP");
    texture.draw(render_text);

    int hp = player->get_modified_property("hp").as<int>();
    int max_hp = player->get_modified_property("maxhp").as<int>();

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
    texture.draw(render_text);

    // Render attributes
    render_attribute(texture, "XP", "xp", { origin.x, origin.y + (2 * line_spacing) });
    render_attribute(texture, "STR", "attribute_strength", { origin.x + (0 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "VIG", "attribute_vigilance", { origin.x + (1 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "END", "attribute_endurance", { origin.x + (2 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "CHA", "attribute_charisma", { origin.x + (3 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "INT", "attribute_intelligence", { origin.x + (4 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "AGI", "attribute_agility", { origin.x + (5 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "LUC", "attribute_luck", { origin.x + (6 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "VIT", "attribute_vitality", { origin.x + (7 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "AUR", "attribute_aura", { origin.x + (8 * attrib_spacing), origin.y + (3 * line_spacing) });
  }

  return;
}

void StatusArea::render_attribute(sf::RenderTarget& target, std::string abbrev, std::string name, RealVec2 location)
{
  auto& config = Service<IConfigSettings>::get();

  sf::Text render_text;
  sf::Color text_color = config.get("text_color").as<Color>();
  sf::Color text_dim_color = config.get("text_dim_color").as<Color>();
  EntityId player = GAME.get_player();

  // Render STR
  render_text.setFont(the_default_mono_font);
  render_text.setColor(text_dim_color);
  render_text.setCharacterSize(config.get("text_default_size").as<int32_t>());
  render_text.setPosition(location.x, location.y);
  render_text.setString(abbrev + ":");
  target.draw(render_text);

  std::string attr_string = std::to_string(player->get_modified_property(name).as<int>());

  render_text.setColor(text_color);
  render_text.setPosition(location.x + 40, location.y);
  render_text.setString(attr_string);
  target.draw(render_text);
}

std::string StatusArea::get_test_label()
{
  return "Testing GUILabel";
}