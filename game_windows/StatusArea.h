#ifndef STATUSAREA_H
#define STATUSAREA_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include "design_patterns/Object.h"

// Forward declarations
class GameState;

class StatusArea
  :
  public Object
{
public:
  StatusArea(sfg::SFGUI& sfgui,
             sfg::Desktop& desktop,
             std::string name,
             sf::IntRect dimensions,
             GameState& gameState);
  virtual ~StatusArea();

protected:
  //virtual void drawContents_(sf::RenderTexture& texture, int frame);

  virtual bool onEvent(Event const& event) override;

private:
  sfg::SFGUI& m_sfgui;
  sfg::Desktop& m_desktop;
  GameState& m_gameState;

  sfg::Window::Ptr m_window;
  sfg::Table::Ptr m_layout;

  sfg::Label::Ptr m_playerLabel;
  sfg::Label::Ptr m_timeLabel;
  sfg::Label::Ptr m_hpDescLabel;
  sfg::Box::Ptr m_hpBox;
  sfg::Label::Ptr m_hpLabel;
  sfg::Label::Ptr m_hpSlashLabel;
  sfg::Label::Ptr m_hpMaxLabel;
  sfg::Box::Ptr m_statusEffectsBox;
  sfg::Label::Ptr m_xpDescLabel;
  sfg::Label::Ptr m_xpLabel;
  std::vector<sfg::Label::Ptr> m_attribDescLabels;
  std::vector<sfg::Label::Ptr> m_attribLabels;

  // void render_attribute(sf::RenderTarget& target,
  //                       std::string abbrev,
  //                       std::string key,
  //                       RealVec2 location);
};

#endif // STATUSAREA_H
