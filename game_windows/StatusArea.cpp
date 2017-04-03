#include "stdafx.h"

#include "game_windows/StatusArea.h"

#include "GUILabel.h"
#include "game/App.h"
#include "game/GameState.h"
#include "services/IConfigSettings.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"

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
  EntityId player = GAME.getPlayer();
  RealVec2 origin(config.get("window_text_offset_x"),
               config.get("window_text_offset_y"));

  json jtext_color = config.get("text_color");
  json jtext_dim_color = config.get("text_dim_color");
  json jtext_warning_color = config.get("text_warning_color");
  json jtext_danger_color = config.get("text_danger_color");

  Color text_color = Color(jtext_color["r"], jtext_color["g"], jtext_color["b"], 255);
  Color text_dim_color = Color(jtext_dim_color["r"], jtext_dim_color["g"], jtext_dim_color["b"], 255);
  Color text_warning_color = Color(jtext_warning_color["r"], jtext_warning_color["g"], jtext_warning_color["b"], 255);
  Color text_danger_color = Color(jtext_danger_color["r"], jtext_danger_color["g"], jtext_danger_color["b"], 255);
  int text_default_size = config.get("text_default_size");
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
    std::string name = player->getProperName();
    name[0] = std::toupper(name[0], std::locale());

    std::string type = player->getDisplayName();
    type[0] = std::toupper(type[0], std::locale());

    render_text.setString(name + " the " + type);
    texture.draw(render_text);

    // Render HP
    render_text.setFont(the_default_mono_font);
    render_text.setColor(text_dim_color);
    render_text.setPosition({ origin.x, origin.y + line_spacing });
    render_text.setString("HP");
    texture.draw(render_text);

    int hp = player->getModifiedProperty("hp");
    int max_hp = player->getModifiedProperty("maxhp");

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

    render_text.setPosition(origin.x + 200, origin.y);
    render_text.setString(std::to_string(GAME.getGameClock().count()));
    texture.draw(render_text);

    // Render game time
    render_text.setPosition(origin.x + 30, origin.y + line_spacing);
    render_text.setString(hp_string);
    texture.draw(render_text);

    // Render attributes
    render_attribute(texture, "XP", "/xp"_json_pointer, { origin.x, origin.y + (2 * line_spacing) });
    render_attribute(texture, "STR", "/attribute/strength"_json_pointer, { origin.x + (0 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "VIG", "/attribute/vigilance"_json_pointer, { origin.x + (1 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "END", "/attribute/endurance"_json_pointer, { origin.x + (2 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "CHA", "/attribute/charisma"_json_pointer, { origin.x + (3 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "INT", "/attribute/intelligence"_json_pointer, { origin.x + (4 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "AGI", "/attribute/agility"_json_pointer, { origin.x + (5 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "LUC", "/attribute/luck"_json_pointer, { origin.x + (6 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "VIT", "/attribute/vitality"_json_pointer, { origin.x + (7 * attrib_spacing), origin.y + (3 * line_spacing) });
    render_attribute(texture, "AUR", "/attribute/aura"_json_pointer, { origin.x + (8 * attrib_spacing), origin.y + (3 * line_spacing) });
  }

  return;
}

void StatusArea::render_attribute(sf::RenderTarget& target, 
                                  std::string abbrev, 
                                  json::json_pointer name,
                                  RealVec2 location)
{
  auto& config = Service<IConfigSettings>::get();

  sf::Text render_text;
  json jtext_color = config.get("text_color");
  json jtext_dim_color = config.get("text_dim_color");
  Color text_color = Color(jtext_color["r"], jtext_color["g"], jtext_color["b"], 255);
  Color text_dim_color = Color(jtext_dim_color["r"], jtext_dim_color["g"], jtext_dim_color["b"], 255);
  EntityId player = GAME.getPlayer();

  // Render STR
  render_text.setFont(the_default_mono_font);
  render_text.setColor(text_dim_color);
  render_text.setCharacterSize(config.get("text_default_size"));
  render_text.setPosition(location.x, location.y);
  render_text.setString(abbrev + ":");
  target.draw(render_text);

  /// @todo handle other types
  std::string attr_string = std::to_string((player->getModifiedProperty(name, 0)).get<int>());

  render_text.setColor(text_color);
  render_text.setPosition(location.x + 40, location.y);
  render_text.setString(attr_string);
  target.draw(render_text);
}

std::string StatusArea::get_test_label()
{
  return "Testing GUILabel";
}