#include "game_windows/StatusArea.h"

#include <cctype>

#include "components/ComponentManager.h"
#include "config/Settings.h"
#include "entity/EntityFactory.h"
#include "game/App.h"
#include "game/GameState.h"
#include "gui/GUILabel.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemChoreographer.h"
#include "systems/SystemTimekeeper.h"

StatusArea::StatusArea(sfg::SFGUI& sfgui,
                       sfg::Desktop& desktop,
                       std::string name,
                       sf::IntRect dimensions,
                       GameState& gameState)
  :
  Object({}),
  m_sfgui{ sfgui },
  m_desktop{ desktop },
  m_gameState{ gameState }
{
  // Create a FloatRect out of the IntRect dimensions.
  /// @todo Just pass the FloatRect directly instead.
  sf::FloatRect floatDims = sf::FloatRect(dimensions.left,
                                          dimensions.top,
                                          dimensions.width,
                                          dimensions.height);

  m_window = sfg::Window::Create();
  m_window->SetTitle("Player Stats");

  m_layout = sfg::Table::Create();
  m_layout->SetColumnSpacings(0.0f);
  m_layout->SetRowSpacings(0.0f);

  /// @todo Add stuff
  // Configuration of status bar (for now):
  //      |0  |1  |2  |3  |4  |5  |6  |7  |8  |9  |10 |11 |12 |13 |14 |15 |16 |17 |
  //      +-------+-------+-------+-------+-------+-------+-------+-------+-------+
  //  0   |Bob the Player                                 |              Game Time|
  //      +-------+-------+-------+-------+-------+-------+-------+-------+-------+
  //  1   |HP | XX/YY     |                                                       |
  //      +-------+-------+                                         Status Effects+
  //  2   |XP | ZZZZZ     |                                                       |
  //      +-------+-------+-------+-------+-------+-------+-------+-------+-------+
  //  3   |STR| n |VIG| n |END| n |CHA| n |INT| n |AGI| n |LUC| n |VIT| n |AUR| n |
  //      +-------+-------+-------+-------+-------+-------+-------+-------+-------+

  /// @todo Determine player attributes number dynamically.
  unsigned int const numAttributes = 9;

  // Create all of the labels/boxes needed for the status bar.
  m_playerLabel = sfg::Label::Create();
  m_playerLabel->SetAlignment({0.0f, 0.5f});
  m_timeLabel = sfg::Label::Create();
  m_timeLabel->SetAlignment({1.0f, 0.5f});
  m_hpDescLabel = sfg::Label::Create();
  m_hpDescLabel->SetAlignment({0.0f, 0.5f});
  m_hpDescLabel->SetText("HP");
  m_hpBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
  m_hpLabel = sfg::Label::Create();
  m_hpLabel->SetAlignment({1.0f, 0.5f});
  m_hpSlashLabel = sfg::Label::Create();
  m_hpSlashLabel->SetText("/");
  m_hpMaxLabel = sfg::Label::Create();
  m_hpMaxLabel->SetAlignment({0.0f, 0.5f});
  m_statusEffectsBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
  m_xpDescLabel = sfg::Label::Create();
  m_xpDescLabel->SetAlignment({0.0f, 0.5f});
  m_xpDescLabel->SetText("XP");
  m_xpLabel = sfg::Label::Create();
  for (auto num = 0; num < numAttributes * 2; ++num)
  {
    auto descLabel = sfg::Label::Create();
    descLabel->SetAlignment({0.0f, 0.5f});
    auto valueLabel = sfg::Label::Create();
    m_attribDescLabels.push_back(descLabel);
    m_attribLabels.push_back(valueLabel);
  }

  // TEMPORARY: Give them temporary filler text.
  /// @todo Remove temporary filler text.
  m_playerLabel->SetText("Clongus Burpo the Idiot");
  m_timeLabel->SetText("(game time goes here)");
  m_hpLabel->SetText("10");
  m_hpMaxLabel->SetText("20");
  auto fillerStatusEffectsLabel = sfg::Label::Create();
  fillerStatusEffectsLabel->SetAlignment({1.0f, 0.5f});
  fillerStatusEffectsLabel->SetText("(status effect icons go here)");
  m_statusEffectsBox->Pack(fillerStatusEffectsLabel, true, true);
  m_xpLabel->SetText("3000");
  for (auto num = 0; num < numAttributes; ++num)
  {
    m_attribLabels[num]->SetText("?");
  }

  // Set attribute label names.
  /// @todo Determine these dynamically from GameRules
  m_attribDescLabels[0]->SetText("STR");
  m_attribDescLabels[1]->SetText("VIG");
  m_attribDescLabels[2]->SetText("END");
  m_attribDescLabels[3]->SetText("CHA");
  m_attribDescLabels[4]->SetText("INT");
  m_attribDescLabels[5]->SetText("AGI");
  m_attribDescLabels[6]->SetText("LUC");
  m_attribDescLabels[7]->SetText("VIT");
  m_attribDescLabels[8]->SetText("AUR");

  // Place HP labels inside the HP box.
  m_hpBox->Pack(m_hpLabel, true, true);
  m_hpBox->Pack(m_hpSlashLabel, false, true);
  m_hpBox->Pack(m_hpMaxLabel, true, true);

  // Place controls in the right spots in the table.
  auto const fillAndExpand = sfg::Table::FILL | sfg::Table::EXPAND;
  auto const fill = sfg::Table::FILL;
  auto const padding = sf::Vector2f(2.f, 1.f);

  m_layout->Attach(m_playerLabel, {0, 0, 12, 1}, fillAndExpand, fill, padding);
  m_layout->Attach(m_timeLabel, {12, 0, (numAttributes * 2) - 12, 1}, fillAndExpand, fill, padding);
  m_layout->Attach(m_hpDescLabel, {0, 1, 1, 1}, fill, fill, padding);
  m_layout->Attach(m_hpBox, {1, 1, 3, 1}, fillAndExpand, fill, padding);
  m_layout->Attach(m_statusEffectsBox, {4, 1, (numAttributes * 2) - 4, 2}, fillAndExpand, fill, padding);
  m_layout->Attach(m_xpDescLabel, {0, 2, 1, 1}, fill, fill, padding);
  m_layout->Attach(m_xpLabel, {1, 2, 3, 1}, fillAndExpand, fill, padding);
  for (auto num = 0; num < numAttributes; ++num)
  {
    m_layout->Attach(m_attribDescLabels[num], {num * 2U, 3, 1, 1}, fill, fill, padding);
    m_layout->Attach(m_attribLabels[num], {(num * 2U) + 1U, 3, 1, 1}, fillAndExpand, fill, padding);
  }

  // Add the table to the window, and the window to the desktop.
  m_window->Add(m_layout);
  m_desktop.Add(m_window);

  // Set initial window size. (Has to be done after a widget is added to a hierarchy.)
  m_window->SetAllocation(floatDims);


  // Add requisite observers.
  subscribeTo(SYSTEMS.timekeeper(), Systems::Timekeeper::EventClockChanged::id);
  subscribeTo(SYSTEMS.choreographer(), Systems::Choreographer::EventPlayerChanged::id);
}

StatusArea::~StatusArea()
{
}

// void StatusArea::drawContents_(sf::RenderTexture& texture, int frame)
// {
//   auto& config = Config::settings();
//   auto& components = m_gameState.components();

//   sf::IntRect pane_dims = getRelativeDimensions();
//   EntityId player = components.globals.player();
//   RealVec2 origin = config.get("window-text-offset");

//   Color text_color = config.get("text-color");
//   Color text_dim_color = config.get("text-dim-color");
//   Color text_warning_color = config.get("text-warning-color");
//   Color text_danger_color = config.get("text-danger-color");
//   int text_default_size = config.get("text-default-size");
//   float line_spacing = the_default_font.getLineSpacing(text_default_size) + 3.0f;
//   float attrib_spacing = 75.0f;

//   // Text offsets relative to the background rectangle.
//   sf::Text render_text;
//   render_text.setFont(the_default_font);
//   render_text.setFillColor(text_color);
//   render_text.setCharacterSize(text_default_size);
//   render_text.setPosition(origin);

//   if (player != EntityId::Void)
//   {
//     // Render player name
//     std::string name = components.properName.valueOr(player, "Player");
//     name[0] = std::toupper(name[0], std::locale());

//     std::string type = components.category.of(player);
//     type[0] = std::toupper(type[0], std::locale());

//     render_text.setString(name + " the " + type);
//     texture.draw(render_text);

//     // Render HP
//     render_text.setFont(the_default_mono_font);
//     render_text.setFillColor(text_dim_color);
//     render_text.setPosition({ origin.x, origin.y + line_spacing });
//     render_text.setString("HP");
//     texture.draw(render_text);

//     bool playerHasHP = components.health.existsFor(player); // If this is false something is DEFINITELY hosed
//     int hp = (playerHasHP ? components.health.of(player).hp() : 1);
//     int max_hp = (playerHasHP ? components.health.of(player).maxHp() : 1);

//     float hp_percentage = static_cast<float>(hp) / static_cast<float>(max_hp);

//     if (hp_percentage > 0.6)
//     {
//       render_text.setFillColor(text_color);
//     }
//     else if (hp_percentage > 0.3)
//     {
//       render_text.setFillColor(text_warning_color);
//     }
//     else
//     {
//       render_text.setFillColor(text_danger_color);
//     }

//     std::string hp_string = boost::lexical_cast<std::string>(hp) + "/" + boost::lexical_cast<std::string>(max_hp);

//     render_text.setPosition(origin.x + 200, origin.y);
//     render_text.setString(std::to_string(SYSTEMS.timekeeper().clock()));
//     texture.draw(render_text);

//     // Render game time
//     render_text.setPosition(origin.x + 30, origin.y + line_spacing);
//     render_text.setString(hp_string);
//     texture.draw(render_text);

//     // Render attributes
//     render_attribute(texture, "XP", "xp", { origin.x, origin.y + (2 * line_spacing) });
//     render_attribute(texture, "STR", "attribute-strength", { origin.x + (0 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "VIG", "attribute-vigilance", { origin.x + (1 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "END", "attribute-endurance", { origin.x + (2 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "CHA", "attribute-charisma", { origin.x + (3 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "INT", "attribute-intelligence", { origin.x + (4 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "AGI", "attribute-agility", { origin.x + (5 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "LUC", "attribute-luck", { origin.x + (6 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "VIT", "attribute-vitality", { origin.x + (7 * attrib_spacing), origin.y + (3 * line_spacing) });
//     render_attribute(texture, "AUR", "attribute-aura", { origin.x + (8 * attrib_spacing), origin.y + (3 * line_spacing) });
//   }

//   return;
// }

bool StatusArea::onEvent(Event const & event)
{
  auto id = event.getId();
  if ((id == GameState::EventClockChanged::id) ||
      (id == Systems::Choreographer::EventPlayerChanged::id))
  {
    /// @todo REIMPLEMENT ME
    //flagForRedraw();
  }

  return false;
}

// void StatusArea::render_attribute(sf::RenderTarget& target,
//                                   std::string abbrev,
//                                   std::string key,
//                                   RealVec2 location)
// {
//   auto& config = Config::settings();

//   sf::Text render_text;
//   Color text_color = config.get("text-color");
//   Color text_dim_color = config.get("text-dim-color");
//   EntityId player = m_gameState.components().globals.player();

//   // Render attribute
//   render_text.setFont(the_default_mono_font);
//   render_text.setFillColor(text_dim_color);
//   render_text.setCharacterSize(config.get("text-default-size"));
//   render_text.setPosition(location.x, location.y);
//   render_text.setString(abbrev + ":");
//   target.draw(render_text);

//   /// @todo reimplement me
//   std::string attr_string = "???"; // std::to_string((player->getModifiedProperty(key, 0)).get<int>());

//   render_text.setFillColor(text_color);
//   render_text.setPosition(location.x + 40, location.y);
//   render_text.setString(attr_string);
//   target.draw(render_text);
// }
